// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInventoryComponent.h"

#include "Camera.h"

void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

AActor* UCharacterInventoryComponent::NextHandItem()
{
	return SetHandItemIndex(CurrentHandItemIndex + 1);
}

AActor* UCharacterInventoryComponent::PreviousHandItem()
{
	return SetHandItemIndex(CurrentHandItemIndex - 1);
}

AActor* UCharacterInventoryComponent::SetHandItemIndex(int32 InItemIndex)
{
	if (IsLocked())
		return GetCurrentHandItem();
	
	if (Items.IsValidIndex(InItemIndex))
	{
		int32 PreviousHandItemIndex = CurrentHandItemIndex;
		AActor* PreviousHandItem = GetCurrentHandItem();
		CurrentHandItemIndex = InItemIndex;

		OnHandItemChanged.Broadcast(
			this,
			GetCurrentHandItem(),
			CurrentHandItemIndex,
			PreviousHandItem,
			PreviousHandItemIndex);
	}
	
	return GetCurrentHandItem();
}

int32 UCharacterInventoryComponent::AddItem(AActor* Actor)
{
	const auto Index = Super::AddItem(Actor);
	SetHandItemIndex(Index);

	return Index;
}
