// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// Prefill with null.
	if (bStaticSize)
		while (Items.Num() < MaxItems)
			Items.Add(nullptr);
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UInventoryComponent::AddItem(AActor* Actor)
{
	if (IsLocked())
		return -1;
	
	if (!IsValid(Actor))
		return -1;

	// Inventory doesn't shrink or expand, uses empty values instead.
	if (bStaticSize)
	{
		for (int32 i = 0; i < GetMaxItems(); i++)
		{
			if (!IsValid(Items[i]))
			{
				Items[i] = Actor;
				OnItemAdded.Broadcast(this, Actor, i);
				return i;
			}
		}
	}
	else
	{
		if (IsFull())
			return -1;
		
		if (const int32 Index = Items.Add(Actor); Index > -1)
		{
			OnItemAdded.Broadcast(this, Actor, Index);
			return Index;
		}
	}

	return -1;
}

bool UInventoryComponent::AddItemAt(AActor* Actor, int32 Index)
{
	if (IsLocked())
		return false;
	
	if (IsValid(Actor))
	{
		Items.EmplaceAt(Index, Actor);
		OnItemAdded.Broadcast(this, Actor, Index);
		return true;
	}

	return false; 
}

int32 UInventoryComponent::AddItemByClass(TSubclassOf<AActor> ActorClass)
{
	if (IsLocked())
		return -1;

	if (IsFull())
		return -1;
	
	// Out of bounds somewhere.
	const auto SpawnLocation = FVector(10000, -10000, 0);
	AActor* SpawnedActor = GetWorld()->SpawnActor(ActorClass, &SpawnLocation);

	return AddItem(SpawnedActor);
}

bool UInventoryComponent::ContainsItem(AActor* Actor) const
{
	if (Items.Contains(Actor))
		if (IsValid(Actor))
			return true;
	
	return false;
}

bool UInventoryComponent::ContainsItemByClass(TSubclassOf<AActor> ActorClass) const
{
	return GetIndexOfItemByClass(ActorClass) > -1;
}

AActor* UInventoryComponent::RemoveItem(AActor* Actor)
{
	if (IsLocked())
		return nullptr;
	
	int32 Index;
	if (Items.Find(Actor, OUT Index))
		return RemoveItem_Internal(Index);

	return nullptr;
}

AActor* UInventoryComponent::RemoveItemByIndex(int32 ItemIndex)
{
	if (IsLocked())
		return nullptr;
	
	if (AActor* FoundActor = GetItemByIndex(ItemIndex))
		return RemoveItem_Internal(ItemIndex);

	return nullptr;
}

AActor* UInventoryComponent::RemoveSingleItemByClass(TSubclassOf<AActor> ActorClass)
{
	if (IsLocked())
		return nullptr;
	
	const int32 FoundIndex = GetIndexOfItemByClass(ActorClass);
	if (FoundIndex > -1)
		return RemoveItem_Internal(FoundIndex);

	return nullptr;
}

int32 UInventoryComponent::RemoveAllItemsByClass(TSubclassOf<AActor> ActorClass)
{
	if (IsLocked())
		return 0;
	
	int32 ItemsRemoved = 0;
	if (IsValid(ActorClass))
	{
		for (int32 i = Items.Num() - 1; i > 0; i--)
		{
			if (IsValid(Items[i]) && Items[i]->GetClass() == ActorClass)
			{
				ItemsRemoved++;
				RemoveItem_Internal(i);
			}
		}

		Items.Shrink();
	}

	return ItemsRemoved;
}

void UInventoryComponent::RemoveAllItems()
{
	if (IsLocked())
		return;

	if (bStaticSize)
	{
		for (int32 i = Items.Num() - 1; i > 0; i--)
			RemoveItem_Internal(i);
	}
	else
	{
		Items.Empty();
	}
	
	OnAllItemsRemoved.Broadcast(this);
}

int32 UInventoryComponent::GetIndexOfItem(AActor* Actor) const
{
	int32 Index;
	if (Items.Find(Actor, OUT Index))
		if (IsValid(Actor))
			return Index;
	
	return -1;
}

int32 UInventoryComponent::GetIndexOfItemByClass(TSubclassOf<AActor> ActorClass) const
{
	if (IsValid(ActorClass))
		for (int32 i = 0; i < Items.Num(); i++)
			if (IsValid(Items[i]) && Items[i]->GetClass() == ActorClass)
				return i;

	return -1;
}

AActor* UInventoryComponent::GetItemByClass(TSubclassOf<AActor> ActorClass)
{
	const int32 Index = GetIndexOfItemByClass(ActorClass);
	if (Index > -1)
		return Items[Index];

	return nullptr;
}

AActor* UInventoryComponent::GetItemByIndex(int32 ItemIndex) const
{
	 return Items.IsValidIndex(ItemIndex) ? Items[ItemIndex] : nullptr;
}

AActor* UInventoryComponent::WithdrawItemByClass(TSubclassOf<AActor> ActorClass)
{
	if (AActor* Actor = RemoveSingleItemByClass(ActorClass); IsValid(Actor))
	{
		Actor->SetActorEnableCollision(true);
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorTickEnabled(true);

		return Actor;
	}

	return nullptr;
}

bool UInventoryComponent::DestroyItemByClass(TSubclassOf<AActor> ActorClass)
{
	if (AActor* Actor = RemoveSingleItemByClass(ActorClass); IsValid(Actor))
	{
		Actor->Destroy();
		return true;
	}

	return false;
}

int32 UInventoryComponent::DepositItem(AActor* Actor)
{
	if (const int32 Index = AddItem(Actor) > -1)
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorLocation(FVector(10000, -10000, 0));
		Actor->SetActorTickEnabled(false);
		return Index;
	}
	
	return -1;
}

int32 UInventoryComponent::TransferItemByClass(UInventoryComponent* NewInventory, TSubclassOf<AActor> ActorClass)
{
	if (IsValid(NewInventory) && !NewInventory->IsLocked())
		if (const auto FoundActor = RemoveSingleItemByClass(ActorClass))
			return NewInventory->AddItem(FoundActor);

	return -1;
}

int32 UInventoryComponent::TransferItemByIndex(UInventoryComponent* NewInventory, int32 ItemIndex)
{
	if (IsValid(NewInventory) && !NewInventory->IsLocked() && !NewInventory->IsFull())
		if (const auto FoundActor = RemoveItemByIndex(ItemIndex))
			return NewInventory->AddItem(FoundActor);

	return -1;
}

AActor* UInventoryComponent::RemoveItem_Internal(int32 Index)
{
	AActor* FoundActor = Items[Index];
	
	if (bStaticSize)
		Items[Index] = nullptr;
	else
		Items.RemoveAt(Index, 1, false);

	OnItemRemoved.Broadcast(this, FoundActor, Index);
	
	return FoundActor;
}

