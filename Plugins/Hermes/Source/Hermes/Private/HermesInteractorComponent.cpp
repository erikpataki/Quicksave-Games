// © 2021 Vixentail, Liam Hall and Savir Sohal


#include "HermesInteractorComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

// Sets default values for this component's properties
UHermesInteractorComponent::UHermesInteractorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	AreaClass = nullptr;
	CanCharacterStepUpOn = ECB_No;
	// ...
	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionObjectType(INTERACTABLE_CHANNEL);
	UPrimitiveComponent::SetCollisionResponseToChannel(INTERACTABLE_CHANNEL, ECR_Overlap);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


// Called when the game starts
void UHermesInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UHermesInteractorComponent::HandleBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UHermesInteractorComponent::HandleEndOverlap);
}

// Called every frame
void UHermesInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHermesInteractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST( UHermesInteractorComponent, bEnabled, SharedParams );
}

void UHermesInteractorComponent::Enable(bool bReset)
{
	// Doesn't care whether client or server calls Enable/Disable. Just keep in mind that if the server does call it,
	// it will be replicated to clients too.
	
	// Already enabled; ignore.
	if (!bReset && IsEnabled())
		return;

	MARK_PROPERTY_DIRTY_FROM_NAME( UHermesInteractorComponent, bEnabled, this );
	bEnabled = true;
	OnRep_IsEnabled();
}

void UHermesInteractorComponent::Disable()
{
	// Already disabled; ignore.
	if (!IsEnabled())
		return;

	MARK_PROPERTY_DIRTY_FROM_NAME( UHermesInteractorComponent, bEnabled, this );
	bEnabled = false;
	OnRep_IsEnabled();
}

void UHermesInteractorComponent::SetEnabled(bool bNewEnabled, bool bReset)
{
	if (bNewEnabled)
		Enable(bReset);
	else
		Disable();
}

void UHermesInteractorComponent::OnRep_IsEnabled()
{
	if (IsEnabled())
	{
		SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AvailableInteractableComps.Empty(5);
		CancelInteractionImplementation();
		SetFocusedInteractableComponent(nullptr);
	}

	OnEnabledChanged.Broadcast(this, IsEnabled());
}

void UHermesInteractorComponent::HandleInteractableComponentOnEnter(
	UHermesInteractableComponent* NewInteractableComponent)
{
	if (!IsValid(NewInteractableComponent))
		return;

	// Used for certain use-cases where you want the server or client to do something when player walks over something but
	// don't necessarily want auto-pickup.
	// I.e. it is used for Equipment to determine whether the ammo in the dropped Equipment should be
	// transferred to the player's current Equipment.
	// There's probably a better way to implement this behaviour but oh well.
	NewInteractableComponent->NotifyOverlap(GetOwner());

	if (GetOwner()->HasLocalNetOwner())
	{
		// If the Comp uses Auto interact, then Focus instantly. Otherwise, keep reference for it so we can Focus on it
		// when suitable.
		if (NewInteractableComponent->GetInteractMode() == EHermesInteractMode::Manual)
		{
			AddFocusableInteractableComponent(NewInteractableComponent);
		}
		else if (NewInteractableComponent->GetInteractMode() == EHermesInteractMode::Auto
			&& NewInteractableComponent->IsEnabled())
		{
			ServerCompleteInteract(NewInteractableComponent);
		}
	}
}

void UHermesInteractorComponent::HandleInteractableComponentOnExit(UHermesInteractableComponent* NewInteractableComponent)
{
	RemoveFocusableInteractableComponent(NewInteractableComponent);
}

void UHermesInteractorComponent::AddFocusableInteractableComponent(
	UHermesInteractableComponent* NewInteractableComponent)
{
	if (!IsValid(NewInteractableComponent))
		return;
	
	// Add to Focusable Comps Array.
	AvailableInteractableComps.Add(NewInteractableComponent);

	// If this Interactor Comp is not currently 'interacting' with something, select the closest interactable comp
	// with the highest priority from the array.
	if (!IsInteracting())
		SetFocusedInteractableComponent(GetMostSuitableFocusableInteractComponent());
}

void UHermesInteractorComponent::RemoveFocusableInteractableComponent(
	UHermesInteractableComponent* NewInteractableComponent)
{	
	// Remove the Interactable Comp from the array.
	if (AvailableInteractableComps.Remove(NewInteractableComponent) > 0)
	{
		// If we were focusing this comp, cancel any interaction, and select the closest interactable comp
		// with the highest priority from the array.
		if (GetFocusedComponent() == NewInteractableComponent)
		{
			CancelInteractionImplementation();
		
			SetFocusedInteractableComponent(GetMostSuitableFocusableInteractComponent());
		}
	}
}

UHermesInteractableComponent* UHermesInteractorComponent::GetInteractingComponent() const
{
	if (IsInteracting())
		return GetFocusedComponent();

	return nullptr;
}

void UHermesInteractorComponent::StartInteraction()
{
	// Already interacting...
	if (IsInteracting())
		return;
	
	if (IsValid(FocusedInteractableComp))
	{
		// Start timer that completes interaction.
		// If time too low, just instantly interact with the comp.
		if (FocusedInteractableComp->GetInteractTime() <= 0.f)
		{
			FocusedInteractableComp->Interact(GetOwner());
		}
		else
		{
			FocusedInteractableComp->StartInteract(GetOwner());
		}
	}
}

void UHermesInteractorComponent::CancelInteraction()
{
	if (CancelInteractionImplementation())
	{
		SetFocusedInteractableComponent(GetMostSuitableFocusableInteractComponent());
	}
}

bool UHermesInteractorComponent::CancelInteractionImplementation()
{
	if (IsInteracting())
	{
		if (IsValid(FocusedInteractableComp))
			FocusedInteractableComp->CancelInteract(GetOwner());

		GetWorld()->GetTimerManager().ClearTimer(InteractTimer);

		OnInteractionCancelled.Broadcast(this, FocusedInteractableComp);

		return true;
	}

	return false;
}

void UHermesInteractorComponent::CompleteInteract()
{
	if (IsValid(FocusedInteractableComp))
		ServerCompleteInteract(FocusedInteractableComp);
}

void UHermesInteractorComponent::ServerCompleteInteract_Implementation(
	UHermesInteractableComponent* InteractableComponent)
{
	// Due to net ownership, Interaction RPCs can only work if the client is the owner. Sadly, this means we can't call
	// RPCs directly on the Interactable Comp since the client will likely not have ownership.
	if (IsValid(InteractableComponent))
		InteractableComponent->Interact(GetOwner());
}

UHermesInteractableComponent* UHermesInteractorComponent::GetMostSuitableFocusableInteractComponent()
{
	UHermesInteractableComponent* MostSuitableComp = nullptr;
	float MostSuitableCompDistance = -1.f;
	EHermesInteractPriority MostSuitableCompPriority = EHermesInteractPriority::Low;
	
	// Select the closest interactable comp with the highest priority from the array.
	for (UHermesInteractableComponent* InteractableComp : AvailableInteractableComps)
	{
		// This comp is invalid; ignore it.
		if (!IsValid(InteractableComp))
			continue;

		// This comp has lower priority; ignore it.
		if (InteractableComp->GetInteractPriority() < MostSuitableCompPriority)
			continue;

		// Basic distance check using origin. Doesn't consider distance between the extents. May upgrade later.
		const float Distance = UKismetMathLibrary::Vector_Distance(
			GetComponentLocation(), InteractableComp->GetComponentLocation());
		
		// There is no suitable comp so this automatically becomes most suitable.
		if (MostSuitableComp == nullptr)
		{
			MostSuitableComp = InteractableComp;
			MostSuitableCompDistance = Distance;
			MostSuitableCompPriority = InteractableComp->GetInteractPriority();
			continue;
		}

		// This comp has higher priority so becomes more suitable.
		if (InteractableComp->GetInteractPriority() > MostSuitableCompPriority)
		{
			MostSuitableComp = InteractableComp;
			MostSuitableCompDistance = Distance;
			MostSuitableCompPriority = InteractableComp->GetInteractPriority();
			continue;
		}

		// This comp has same priority but has lower distance so becomes more suitable.
		if (Distance < MostSuitableCompDistance)
		{
			MostSuitableComp = InteractableComp;
			MostSuitableCompDistance = Distance;
		}
	}

	return MostSuitableComp;
}

void UHermesInteractorComponent::SetFocusedInteractableComponent(UHermesInteractableComponent* NewInteractableComponent)
{	
	// We are replacing an existing focused comp.
	if (IsValid(FocusedInteractableComp) && FocusedInteractableComp != NewInteractableComponent)
	{
		// Unsubscribe
		FocusedInteractableComp->OnInteractionStarted.RemoveDynamic(
			this, &UHermesInteractorComponent::HandleOnFocusedInteractionStarted);
		FocusedInteractableComp->OnInteractionCancelled.RemoveDynamic(
			this, &UHermesInteractorComponent::HandleOnFocusedInteractionCancelled);
		FocusedInteractableComp->OnInteractionComplete.RemoveDynamic(
			this, &UHermesInteractorComponent::HandleOnFocusedInteractionComplete);
		
		OnUnfocus.Broadcast(this, FocusedInteractableComp);
	}
	
	if (IsValid(NewInteractableComponent))
	{
		if (NewInteractableComponent->Focus(GetOwner()))
		{
			FocusedInteractableComp = NewInteractableComponent;

			// Subscribe
			FocusedInteractableComp->OnInteractionStarted.AddUniqueDynamic(
				this, &UHermesInteractorComponent::HandleOnFocusedInteractionStarted);
			FocusedInteractableComp->OnInteractionCancelled.AddUniqueDynamic(
				this, &UHermesInteractorComponent::HandleOnFocusedInteractionCancelled);
			FocusedInteractableComp->OnInteractionComplete.AddUniqueDynamic(
				this, &UHermesInteractorComponent::HandleOnFocusedInteractionComplete);
			
			OnFocus.Broadcast(this, FocusedInteractableComp);
		}
	}
}

void UHermesInteractorComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UHermesInteractableComponent* InteractableComponent = Cast<UHermesInteractableComponent>(OtherComp);
	if (IsValid(InteractableComponent))
	{
		HandleInteractableComponentOnEnter(InteractableComponent);
	}
}

void UHermesInteractorComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// I wonder. Would this get triggered if the object is destroyed?
	// If so, it will be null so I won't be able to remove it. Maybe if it is null, remove all null refs in array
	// and if focus is now also null, find new comp to focus.

	HandleInteractableComponentOnExit(Cast<UHermesInteractableComponent>(OtherComp));
}

void UHermesInteractorComponent::HandleOnFocusedInteractionStarted(const UHermesInteractableComponent* Sender,
	const AActor* Instigator)
{
	// Just in case.
	if (!IsValid(Sender) || Sender != GetFocusedComponent())
		return;

	GetWorld()->GetTimerManager().SetTimer(
		InteractTimer, this, &UHermesInteractorComponent::CompleteInteract,
		Sender->GetInteractTime());
	
	OnInteractionStarted.Broadcast(this, Sender);
}

void UHermesInteractorComponent::HandleOnFocusedInteractionComplete(const UHermesInteractableComponent* Sender,
	const AActor* Instigator)
{
	// Just in case.
	if (!IsValid(Sender) || Sender != GetFocusedComponent())
		return;
	
	// We completed the interaction.
	if (Instigator == GetOwner())
	{
		// No longer interacting so clear.
		GetWorld()->GetTimerManager().ClearTimer(InteractTimer);
		
		OnInteractionComplete.Broadcast(this, Sender);
	}

	// Interactable Comp is no longer enabled; remove from focusable comps.
	if (!Sender->IsEnabled())
	{
		RemoveFocusableInteractableComponent(FocusedInteractableComp);
	}
}

void UHermesInteractorComponent::HandleOnFocusedInteractionCancelled(const UHermesInteractableComponent* Sender,
	const AActor* Instigator)
{
	// Just in case.
	/*if (Sender != GetFocusedComponent())
		return;
	
	OnInteractionCancelled.Broadcast(this, Sender);*/
}

