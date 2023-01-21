// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROGWATER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, Category=Inventory)
	TArray<TSubclassOf<AActor>> InitialItems;

	UPROPERTY(EditAnywhere, Category=Inventory, meta=(ClampMin=0, ClampMax=100))
	int32 MaxItems;

	UPROPERTY(EditAnywhere, Category=Inventory)
	bool bLocked = false;

	UPROPERTY(EditAnywhere, Category=Inventory)
	bool bStaticSize = false;
	
	UPROPERTY(VisibleAnywhere, Category=Inventory)
	TArray<AActor*> Items;

public:
	UFUNCTION(BlueprintPure, Category=Inventory)
	const TArray<AActor*>& GetItems() const { return Items; }

	UFUNCTION(BlueprintPure, Category=Inventory)
	bool IsFull() const { return GetItemCount() >= GetMaxItems(); }

	UFUNCTION(BlueprintPure, Category=Inventory)
	bool IsLocked() const { return bLocked; }

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void SetLocked(bool bInLock) { bLocked = bInLock; }

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void SetStaticSize(bool bStatic) { if (HasBegunPlay()) return; bStaticSize = bStatic; }

	UFUNCTION(BlueprintPure, Category=Inventory)
	int32 GetMaxItems() const { return MaxItems; }

	UFUNCTION(BlueprintPure, Category=Inventory)
	int32 GetItemCount() const { return Items.Num(); }

	// May want to use DepositItem instead as it does additional processing, such as disabling the actor.
	UFUNCTION(BlueprintCallable, Category=Inventory)
	virtual int32 AddItem(AActor* Actor);

	// Do not use. Untested.
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool AddItemAt(AActor* Actor, int32 Index);
	
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 AddItemByClass(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintPure, Category=Inventory)
	bool ContainsItem(AActor* Actor) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	bool ContainsItemByClass(TSubclassOf<AActor> ActorClass) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* RemoveItem(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* RemoveItemByIndex(int32 ItemIndex);

	// May want to use WithdrawItem instead if you used DepositItem.
	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* RemoveSingleItemByClass(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 RemoveAllItemsByClass(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveAllItems();

	UFUNCTION(BlueprintPure, Category=Inventory)
	int32 GetIndexOfItem(AActor* Actor) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	int32 GetIndexOfItemByClass(TSubclassOf<AActor> ActorClass) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	AActor* GetItemByClass(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintPure, Category=Inventory)
	AActor* GetItemByIndex(int32 ItemIndex) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* WithdrawItemByClass(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool DestroyItemByClass(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 DepositItem(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 TransferItemByClass(UInventoryComponent* NewInventory, TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 TransferItemByIndex(UInventoryComponent* NewInventory, int32 ItemIndex);

protected:
	AActor* RemoveItem_Internal(int32 Index);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemAddedSignature,
		const UInventoryComponent*, Sender,
		const AActor*, Item,
		const int32, ItemIndex);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAllItemsRemovedSignature,
		const UInventoryComponent*, Sender);

	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FItemAddedSignature OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FItemAddedSignature OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category=Inventory)
	FAllItemsRemovedSignature OnAllItemsRemoved;
};