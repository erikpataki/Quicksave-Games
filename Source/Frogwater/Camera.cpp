// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera.h"

#include "HermesInteractableComponent.h"
#include "ImageUtils.h"
#include "ScannableComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACamera::ACamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f / 5.f;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());

	PickupCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(StaticMeshComponent);
	
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCaptureComponent->SetupAttachment(StaticMeshComponent);
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->bCaptureOnMovement = false;

	InteractableComponent = CreateDefaultSubobject<UHermesInteractableComponent>(TEXT("Interactable"));
	InteractableComponent->SetBoxExtent(FVector(100, 100, 100));
	InteractableComponent->SetupAttachment(StaticMeshComponent);
}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACamera::Interact_Implementation(AController* InInstigator)
{
	if (GetWorldTimerManager().IsTimerActive(TakePictureCooldownTimer))
		return false;
	
	TakePicture();

	// Set timer to prevent a new picture from being taken so quickly.
	GetWorldTimerManager().SetTimer(TakePictureCooldownTimer, TakePictureCooldownTime, false);
	
	return true;
}

void ACamera::OnOffhand_Implementation(AController* InInstigator)
{
	// Attach to hand and show.
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);

	SceneCaptureComponent->bCaptureOnMovement = true;
	StopTraceTimer();
}

void ACamera::OnHolstered_Implementation(AController* InInstigator)
{
	// Hide.
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);

	SceneCaptureComponent->bCaptureOnMovement = false;
	StopTraceTimer();
}

void ACamera::OnAim_Implementation(AController* InInstigator)
{
	// Move towards centre of screen and show camera HUD.
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);

	SceneCaptureComponent->bCaptureOnMovement = true;
	StartTraceTimer();
}

bool ACamera::InteractRequiresAiming_Implementation()
{
	return true;
}

const UTexture2D* ACamera::GetItemImage_Implementation() const
{
	return nullptr;
}

void ACamera::TakePicture()
{
	// Do a new trace that is most updated.
	Trace();

	if (IsValid(FocusedScannable))
	{
		const bool bSuccess = FocusedScannable->ExecPictureTaken(this);
		OnPictureTaken(bSuccess, FocusedScannable);
		
		return;
	}

	OnPictureTaken(false, nullptr);
}

void ACamera::SetOwningPawn(APawn* InOwningPawn)
{
	OwningPawn = InOwningPawn;
}

void ACamera::Trace()
{
	if (bEnablePinpointTrace)
	{
		// Trace for scannable objects.
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwningPawn());
		
		TArray<FHitResult> HitResults;
		GetWorld()->LineTraceMultiByChannel(
			OUT HitResults,
			GetTraceOrigin(),
			GetTraceEnd(),
			SCANNABLE_CHANNEL,
			Params);

		TArray<FHitResult> VisibilityHitResults;
		GetWorld()->LineTraceMultiByChannel(
			OUT VisibilityHitResults,
			GetTraceOrigin(),
			GetTraceEnd(),
			ECC_Visibility,
			Params);

		RemoveNotVisibleScannables(HitResults, VisibilityHitResults);
		
		auto Comp = GetHighestPriorityScannable(HitResults);

		if (IsValid(Comp))
		{
			MarkScannableComponent(Comp);
			return;
		}
	}

	for (int32 i = 1; i < TraceResolution; i++)
	{
		const float CurrentRadius = (TraceMaxRadius / TraceResolution) * i;
		const auto Comp = TraceForBestScannable(CurrentRadius);
		
		if (IsValid(Comp))
		{
			MarkScannableComponent(Comp);
			return;
		}
	}
}

FVector ACamera::GetTraceOrigin() const
{
	return bUsePawnViewport && GetOwningPawn() ? GetOwningPawn()->GetPawnViewLocation() : GetActorLocation();
}

FVector ACamera::GetTraceEnd() const
{
	if (bUsePawnViewport && GetOwningPawn())
		return GetTraceOrigin() + (UKismetMathLibrary::GetForwardVector(GetOwningPawn()->GetViewRotation()) * TraceMaxDistance);

	return GetTraceOrigin() + GetActorForwardVector() * TraceMaxDistance;
}

UScannableComponent* ACamera::TraceForBestScannable(float Radius)
{
	TArray<FHitResult> ScanHitResults = TraceForScanChannel(Radius);
	const TArray<FHitResult> VisibilityHitResults = TraceForVisibilityChannel(Radius);
	RemoveNotVisibleScannables(IN OUT ScanHitResults, VisibilityHitResults);
	
	return GetHighestPriorityScannable(ScanHitResults);
}

TArray<FHitResult> ACamera::TraceForScanChannel(float Radius)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwningPawn());
	
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMulti(
		this,
		GetTraceOrigin(),
		GetTraceEnd(),
		FVector(Radius),
		FRotator(), /* rotation on camera is not currently a design choice */
		UEngineTypes::ConvertToTraceType(SCANNABLE_CHANNEL),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		OUT HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3.f);

	return HitResults;
}

TArray<FHitResult> ACamera::TraceForVisibilityChannel(float Radius)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwningPawn());
	
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMulti(
		this,
		GetTraceOrigin(),
		GetTraceEnd(),
		FVector(Radius),
		FRotator(), /* rotation on camera is not currently a design choice */
		UEngineTypes::ConvertToTraceType(ECC_Visibility) /* visibility channel */,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OUT HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3.f);

	return HitResults;
}

UScannableComponent* ACamera::GetHighestPriorityScannable(const TArray<FHitResult> HitResults) const
{
	UScannableComponent* HighestPriorityScannable = nullptr;
	
	for (auto& HitResult : HitResults)
		if (const auto Comp = Cast<UScannableComponent>(HitResult.GetComponent()))
			if (!IsValid(HighestPriorityScannable) || Comp->GetPriority() > HighestPriorityScannable->GetPriority())
				HighestPriorityScannable = Comp;
	
	return HighestPriorityScannable;
}

void ACamera::RemoveNotVisibleScannables(TArray<FHitResult>& HitResults, const TArray<FHitResult>& VisibleHitResults) const
{
	// Removes scannables that are not in the visible array from the hit results array.
	for (int32 i = HitResults.Num() - 1; i > 0; i--)
	{
		bool bFound = false;

		for (int32 y = 0; y < VisibleHitResults.Num(); y++)
		{
			if (HitResults[i].GetComponent() == VisibleHitResults[y].GetComponent())
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
			HitResults.RemoveAt(i);
	}
}

void ACamera::MarkScannableComponent(UScannableComponent* ScannableComponent)
{
	FocusedScannable = ScannableComponent;

	if (FocusedScannable)
		FocusedScannable->ExecIsFocus(this);
}

void ACamera::SaveLastCaptureAsFile(FString FileNameWithoutExtension)
{
	UKismetRenderingLibrary::ExportRenderTarget(
		this,
		SceneCaptureComponent->TextureTarget,
		TEXT("K:/QS/Pictures"),
		FileNameWithoutExtension + TEXT(".HDR"));
}

void ACamera::StartTraceTimer()
{
	GetWorldTimerManager().SetTimer(TraceTimer, this, &ACamera::OnTraceTick, TraceTickInterval, true);
}

void ACamera::StopTraceTimer()
{
	GetWorldTimerManager().ClearTimer(TraceTimer);
}

void ACamera::OnTraceTick()
{
	UKismetSystemLibrary::PrintString(this, TEXT("Trace Tick"));
	if (IsValid(GetOwningPawn()))
		Trace();
}

void ACamera::OnSceneCaptureTick()
{
	UKismetSystemLibrary::PrintString(this, TEXT("Capture Tick"));
	SceneCaptureComponent->CaptureScene();
}

void ACamera::OnPictureTaken_Implementation(bool bSuccess, UScannableComponent* ScannableComponent)
{
}
