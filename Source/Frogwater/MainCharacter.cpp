// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "CharacterInventoryComponent.h"
#include "HermesInteractorComponent.h"
#include "Interactable.h"
#include "Camera/CameraComponent.h"
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

	if (GetHandItemState() == EItemState::Aimed)
	{
		if (const auto CurrentHandItem = InventoryComponent->GetCurrentHandItem(); IsValid(CurrentHandItem))
		{
			// Calls Interact on any object with the IInteractable interface (should be every item).
			if (CurrentHandItem->Implements<UInteractable>())
				bool bSuccess = IInteractable::Execute_Interact(CurrentHandItem, GetController());
		}
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

AActor* AMainCharacter::SwitchToNextItem()
{
	return InventoryComponent->NextHandItem();
}

AActor* AMainCharacter::SwitchToPreviousItem()
{
	return InventoryComponent->PreviousHandItem();
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
