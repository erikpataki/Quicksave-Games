// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "CharacterInventoryComponent.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API UCharacterInventoryComponent : public UInventoryComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditAnywhere, Category=Inventory)
	bool bAutoSwitchToNewHandItem = true;
	
	UPROPERTY(VisibleAnywhere, Category=Inventory)
	int32 CurrentHandItemIndex = -1;

public:
	UFUNCTION(BlueprintPure, Category=Inventory)
	AActor* GetCurrentHandItem() const { return GetItemByIndex(CurrentHandItemIndex); }

	UFUNCTION(BlueprintPure, Category=Inventory)
	int32 GetCurrentHandItemIndex() const { return CurrentHandItemIndex; }

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* NextHandItem();

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* PreviousHandItem();

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* SetHandItemIndex(int32 InItemIndex);

	virtual int32 AddItem(AActor* Actor) override;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FHandItemChangedSignature,
		const UCharacterInventoryComponent*, Sender,
		const AActor*, NewItem,
		const int32, NewHandItemIndex,
		const AActor*, PreviousItem,
		const int32, PreviousHandItemIndex);

	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FHandItemChangedSignature OnHandItemChanged;
};
