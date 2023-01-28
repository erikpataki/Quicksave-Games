// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "Camera.h"
#include "CharacterInventoryComponent.h"
#include "HermesInteractorComponent.h"
#include "Interactable.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("Inventory"));
	InventoryComponent->SetStaticSize(true);

	InteractorComponent = CreateDefaultSubobject<UHermesInteractorComponent>(TEXT("Interactor"));
	InteractorComponent->SetupAttachment(GetMesh());

	HandRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HandRoot"));
	HandRoot->SetupAttachment(GetFirstPersonCameraComponent());
	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHand"));
	LeftHandMesh->SetupAttachment(HandRoot);
	RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHand"));
	RightHandMesh->SetupAttachment(HandRoot);	
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent->OnHandItemChanged.AddUniqueDynamic(this, &AMainCharacter::OnHandItemChanged);
	OnHandStateChanged.AddUniqueDynamic(this, &AMainCharacter::HandleHandStateChanged);
}

void AMainCharacter::OnPrimaryAction()
{
	Super::OnPrimaryAction();
	
	if (const auto CurrentHandItem = InventoryComponent->GetCurrentHandItem(); IsValid(CurrentHandItem))
	{
		// Calls Interact on any object with the IInteractable interface (should be every item).
		if (CurrentHandItem->Implements<UInteractable>())
		{
			if (GetHandItemState() == EItemState::Aimed || !IInteractable::Execute_InteractRequiresAiming(CurrentHandItem))
				bool bSuccess = IInteractable::Execute_Interact(CurrentHandItem, GetController());
		}
	}
}

void AMainCharacter::Turn(float Rate)
{
	Super::Turn(Rate);
}

void AMainCharacter::LookUp(float Rate)
{
	Super::LookUp(Rate);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TotalTime += DeltaTime;

	if (!FMath::IsNearlyEqual(TargetFov, FirstPersonCameraComponent->FieldOfView, .1f))
	{
		FirstPersonCameraComponent->SetFieldOfView(
			FMath::FInterpTo(FirstPersonCameraComponent->FieldOfView, TargetFov, DeltaTime, FovChangeSpeed));
	}

	UpdateHandSway(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &AMainCharacter::OnPrimaryActionReleased);
	PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &AMainCharacter::OnSecondaryAction);
	PlayerInputComponent->BindAction("NextItem", IE_Pressed, this, &AMainCharacter::SwitchToNextItem_Internal);
	PlayerInputComponent->BindAction("PreviousItem", IE_Pressed, this, &AMainCharacter::SwitchToPreviousItem_Internal);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainCharacter::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMainCharacter::StopInteract);
	PlayerInputComponent->BindAction("ZoomIn", IE_Pressed, this, &AMainCharacter::ZoomIn);
	PlayerInputComponent->BindAction("ZoomOut", IE_Pressed, this, &AMainCharacter::ZoomOut);
}

AActor* AMainCharacter::SwitchToNextItem()
{
	return InventoryComponent->NextHandItem();
}

AActor* AMainCharacter::SwitchToPreviousItem()
{
	return InventoryComponent->PreviousHandItem();
}

void AMainCharacter::SetTargetFov(float NewFov, bool bForceChange)
{
	TargetFov = NewFov;

	if (bForceChange)
		FirstPersonCameraComponent->SetFieldOfView(NewFov);
}

void AMainCharacter::SetHandRootOffset(FVector NewOffset)
{
	HandRootOffset = NewOffset;
}

void AMainCharacter::OnSecondaryAction()
{
	OnSecondaryUseItem.Broadcast();

	// No item = holstered (none) state.
	if (!IsValid(InventoryComponent->GetCurrentHandItem()))
		SetHandState(EItemState::HolsteredOrNone);
	else
	{
		// Switch between off hand and in use state.
		if (HandState == EItemState::Offhand)
			SetHandState(EItemState::Aimed);
		else if (HandState == EItemState::Aimed)
			SetHandState(EItemState::Offhand);
	}
}

void AMainCharacter::OnPrimaryActionReleased()
{
	OnPrimaryActionStop.Broadcast();

	if (GetHandItemState() == EItemState::Aimed)
	{
		if (const auto CurrentHandItem = InventoryComponent->GetCurrentHandItem(); IsValid(CurrentHandItem))
		{
			// Calls Interact on any object with the IInteractable interface (should be every item).
			if (CurrentHandItem->Implements<UInteractable>())
				IInteractable::Execute_StopInteract(CurrentHandItem, GetController());
		}
	}
}

void AMainCharacter::SetHandState(EItemState NewHandState)
{
	if (HandState != NewHandState)
	{
		HandState = NewHandState;
		
		OnHandStateChanged.Broadcast(
			NewHandState,
			InventoryComponent->GetCurrentHandItem(),
		    InventoryComponent->GetCurrentHandItemIndex());
	}
}

void AMainCharacter::OnHandItemChanged(const UCharacterInventoryComponent* Sender, const AActor* NewItem,
	const int32 NewHandItemIndex, const AActor* PreviousItem, const int32 PreviousHandItemIndex)
{
	// Item has changed.
	if (IsValid(PreviousItem) && PreviousItem != NewItem)
		if (PreviousItem->Implements<UInteractable>())
		{
			IInteractable::Execute_StopInteract(const_cast<AActor*>(PreviousItem), GetController());
			IInteractable::Execute_OnHolstered(const_cast<AActor*>(PreviousItem), GetController());
		}

	// If no item, set to hand state to none.
	SetHandState(IsValid(NewItem) ? EItemState::Offhand : EItemState::HolsteredOrNone);

	if (IsValid(NewItem))
	{
		// Execute either Offhand or Holstered on the new item.
		if (GetHandItemState() == EItemState::Offhand)
		{
			if (NewItem->Implements<UInteractable>())
				IInteractable::Execute_OnOffhand(const_cast<AActor*>(NewItem), GetController());
		}
		else if (GetHandItemState() == EItemState::HolsteredOrNone)
		{
			if (NewItem->Implements<UInteractable>())
				IInteractable::Execute_OnHolstered(const_cast<AActor*>(NewItem), GetController());
		}
	}
}

void AMainCharacter::HandleHandStateChanged(const EItemState NewItemState, const AActor* Item,
	const int32 ItemIndex)
{
	SetTargetFov(DefaultFov);
	
	// If new state is Offhand or Aimed, notify the item.
	if (NewItemState == EItemState::Offhand)
	{
		if (Item->Implements<UInteractable>())
		{
			IInteractable::Execute_StopInteract(const_cast<AActor*>(Item), GetController());
			IInteractable::Execute_OnOffhand(const_cast<AActor*>(Item), GetController());
		}
	}
	else if (NewItemState == EItemState::Aimed)
	{
		if (Item->Implements<UInteractable>())
			IInteractable::Execute_OnAim(const_cast<AActor*>(Item), GetController());
	}
}

void AMainCharacter::Interact()
{
	InteractorComponent->StartInteraction();
}

void AMainCharacter::StopInteract()
{
	InteractorComponent->CancelInteraction();
}

void AMainCharacter::ZoomIn_Implementation()
{
	if (GetHandItemState() != EItemState::Aimed)
		return;
	
	if (auto Camera = Cast<ACamera>(GetInventory()->GetCurrentHandItem()); IsValid(Camera))
		Camera->SetZoomMagnification(Camera->GetCurrentZoomMagnification() + ZoomStep);
}

void AMainCharacter::ZoomOut_Implementation()
{
	if (GetHandItemState() != EItemState::Aimed)
		return;
	
	if (auto Camera = Cast<ACamera>(GetInventory()->GetCurrentHandItem()); IsValid(Camera))
		Camera->SetZoomMagnification(Camera->GetCurrentZoomMagnification() - ZoomStep);
}

void AMainCharacter::UpdateHandSway(float DeltaTime) const
{
	FVector TargetLocation;
	if (bEnableBreathingHandSway)
	{
		ApplyBreathingHandSway(IN OUT TargetLocation);

		if (GetHandItemState() == EItemState::Aimed)
			TargetLocation *= HandSwayAimReduction;

		TargetLocation += HandRootOffset;
		
		const auto NewLocation = FMath::VInterpTo(
			GetHandRoot()->GetRelativeLocation(),
			TargetLocation,
			DeltaTime,
			HandSwaySpeed * 2.f);
	
		GetHandRoot()->SetRelativeLocation(NewLocation);
	}
	
	FRotator TargetRotation;

	if (bEnableRotationHandSway)
		ApplyRotationHandSway(IN OUT TargetRotation);
	if (bEnableLocationHandSway)
		ApplyLocationHandSway(IN OUT TargetRotation);

	if (GetHandItemState() == EItemState::Aimed)
		TargetRotation *= HandSwayAimReduction;

	const auto NewRotation = FMath::RInterpTo(
		GetHandRoot()->GetRelativeRotation(),
		TargetRotation,
		DeltaTime,
		HandSwaySpeed);
	
	GetHandRoot()->SetRelativeRotation(NewRotation);
}

void AMainCharacter::ApplyBreathingHandSway(FVector& TargetLocation) const
{
	// Lissajous Curve with an hourglass-like shape.	
	/*TargetLocation.Y += FMath::Sin(
		(2. * TotalTime) + UE_DOUBLE_PI) * BreathingHandSwayMultiplier;

	TargetLocation.Z += 2. * FMath::Sin(
	TotalTime) * BreathingHandSwayMultiplier;*/

	// Lissajous Curve with an hourglass-like shape but a little bit messy.
	TargetLocation.Y += 1.f * FMath::Sin(
		2.f * TotalTime * .5f) * BreathingHandSwayMultiplier;
	
	TargetLocation.Z += 1.f * FMath::Sin(
		3.f * TotalTime * .5f + UE_PI / 2.f) * BreathingHandSwayMultiplier;

}

void AMainCharacter::ApplyRotationHandSway(FRotator& TargetRotation) const
{
	const float Yaw = GetTurn() * RotationHandSwayMultiplier;
	const float Pitch = -(GetLookUp() * RotationHandSwayMultiplier);
	ApplyHandSwayRotation(IN OUT TargetRotation, Pitch, Yaw);
}

void AMainCharacter::ApplyLocationHandSway(FRotator& TargetRotation) const
{
	const auto RightVelocity = GetActorRightVector() * GetVelocity();
	const auto UpVelocity = GetActorUpVector() * GetVelocity();
	const float TotalRightVelocity = RightVelocity.X + RightVelocity.Y + RightVelocity.Z;
	const float TotalUpVelocity = UpVelocity.X + UpVelocity.Y + UpVelocity.Z;
	
	ApplyHandSwayRotation(
		IN OUT TargetRotation,
		TotalUpVelocity * LocationHandSwayMultiplier,
		TotalRightVelocity * LocationHandSwayMultiplier * .33f); 
}

void AMainCharacter::ApplyHandSwayRotation(FRotator& TargetRotation, float Pitch, float Yaw) const
{
	TargetRotation.Pitch = FMath::Clamp(
		TargetRotation.Pitch + Pitch * HandSwayMultiplier.Pitch,
		MinHandSway.Pitch,
		MaxHandSway.Pitch);
	
	TargetRotation.Yaw = FMath::Clamp(
		TargetRotation.Yaw + Yaw * HandSwayMultiplier.Yaw,
		MinHandSway.Yaw,
		MaxHandSway.Yaw);
	
	TargetRotation.Roll = FMath::Clamp(
		TargetRotation.Roll + Yaw * HandSwayMultiplier.Roll,
		MinHandSway.Roll,
		MaxHandSway.Roll);
}
