// © 2021 Vixentail, Liam Hall and Savir Sohal


#include "HermesInteractableComponent.h"

#include "HermesInteractorComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UHermesInteractableComponent::UHermesInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	UActorComponent::SetAutoActivate(true);
	AreaClass = nullptr;
	CanCharacterStepUpOn = ECB_No;
	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionObjectType(INTERACTABLE_CHANNEL);
	UPrimitiveComponent::SetCollisionResponseToChannel(INTERACTABLE_CHANNEL, ECR_Overlap);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// ...
}


// Called when the game starts
void UHermesInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UHermesInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UHermesInteractableComponent::IsEnabled() const
{
	return IsActive() && IsQueryCollisionEnabled() && Mode != EHermesInteractMode::None;
}

void UHermesInteractableComponent::Interact(const AActor* Instigator)
{
	// Item cannot be interacted with if it is not active.
	if (!IsEnabled())
		return;

	// If we do not have ownership (i.e. on client and is a world object), then try to move to server using the
	// Interactor Component since the client will have ownership of this object.
	if (!GetOwner()->HasAuthority() && !GetOwner()->HasLocalNetOwner())
	{
		UHermesInteractorComponent* InteractorComponent = Cast<UHermesInteractorComponent>(
			Instigator->GetComponentByClass(UHermesInteractorComponent::StaticClass()));

		if (IsValid(InteractorComponent))
		{
			InteractorComponent->ServerCompleteInteract(this);
		}
	}
	
	Server_Interact(Instigator);
}

bool UHermesInteractableComponent::StartInteract(AActor* Instigator)
{
	// Item cannot be interacted with if it is not active.
	if (!IsEnabled())
		return false;
	
	OnInteractionStarted.Broadcast(this, Instigator);
	return true;
}

void UHermesInteractableComponent::CancelInteract(AActor* Instigator)
{
	if (!IsValid(Instigator))
		return;
	
	OnInteractionCancelled.Broadcast(this, Instigator);
}

bool UHermesInteractableComponent::Focus(AActor* Instigator)
{
	// Item cannot be interacted with if it is not active.
	if (!IsEnabled())
		return false;

	if (Mode == EHermesInteractMode::Auto)
	{
		Interact(Instigator);
		return false;
	}
	if (Mode == EHermesInteractMode::Manual)
	{
		OnFocus.Broadcast(this, Instigator);
		return true;
	}

	return false;
}

void UHermesInteractableComponent::Unfocus(AActor* Instigator)
{
	OnUnfocus.Broadcast(this, Instigator);
}

void UHermesInteractableComponent::NotifyOverlap(AActor* Instigator)
{
	if (!IsValid(Instigator))
		return;

	OnOverlap.Broadcast(this, Instigator);
}

void UHermesInteractableComponent::Server_Interact_Implementation(const AActor* Instigator)
{
	// Server authentication.
	
	// Server does not recognise that this Comp is overlapping with said Actor.
	//if (!IsOverlappingActor(Instigator))
	//	return;
	
	// Item cannot be interacted with if it is not active.
	if (!IsEnabled())
		return;
	
	SendInteractionComplete(Instigator);

	if (GetDestroyAfterInteraction() == EHermesNetMode::Server)
	{
		GetOwner()->Destroy();
	}
}

void UHermesInteractableComponent::SendInteractionComplete_Implementation(const AActor* Instigator)
{
	OnInteractionComplete.Broadcast(this, Instigator);

	if ((GetOwner()->IsPendingKillPending()) && GetDestroyAfterInteraction() == EHermesNetMode::Multicast
		|| (!GetOwner()->HasAuthority() && GetDestroyAfterInteraction() == EHermesNetMode::Client))
	{
		GetOwner()->Destroy();
	}
}

