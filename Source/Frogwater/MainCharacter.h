// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrogwaterCharacter.h"
#include "ItemState.h"
#include "MainCharacter.generated.h"

class UHermesInteractorComponent;
class UCharacterInventoryComponent;
UCLASS()
class FROGWATER_API AMainCharacter : public AFrogwaterCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	UCharacterInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction)
	UHermesInteractorComponent* InteractorComponent;

	UPROPERTY(EditAnywhere, Category=Interaction)
	USceneComponent* HandRoot;

	UPROPERTY(EditAnywhere, Category=Interaction)
	UStaticMeshComponent* LeftHandMesh;

	UPROPERTY(EditAnywhere, Category=Interaction)
	UStaticMeshComponent* RightHandMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnPrimaryAction() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnSecondaryUseItem;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnPrimaryActionStop;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemStateChangedSignature,
		const EItemState, NewItemState,
		const AActor*, NewItem,
		const int32, NewItemIndex);

	UPROPERTY(BlueprintAssignable, Category=Interaction)
	FItemStateChangedSignature OnHandStateChanged;
	
protected:
	UPROPERTY(VisibleAnywhere, Category=Interaction)
	EItemState HandState;

public:
	UFUNCTION(BlueprintPure, Category=Interaction)
	EItemState GetHandItemState() const { return HandState; }

	UFUNCTION(BlueprintPure, Category=Inventory)
	UCharacterInventoryComponent* GetInventory() const { return InventoryComponent; }
	
	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* SwitchToNextItem();

	UFUNCTION(BlueprintCallable, Category=Inventory)
	AActor* SwitchToPreviousItem();

	UFUNCTION(BlueprintPure, Category=Interaction)
	UHermesInteractorComponent* GetInteractorComponent() const { return InteractorComponent; }

	UFUNCTION(BlueprintPure, Category=Interaction)
	USceneComponent* GetHandRoot() const { return HandRoot; }

	UFUNCTION(BlueprintPure, Category=Interaction)
	UStaticMeshComponent* GetLeftHandMesh() const { return LeftHandMesh; }

	UFUNCTION(BlueprintPure, Category=Interaction)
	UStaticMeshComponent* GetRightHand() const { return RightHandMesh; }

protected:
	UFUNCTION()
	void OnSecondaryAction();

	UFUNCTION()
	void OnPrimaryActionReleased();

	void SetHandState(EItemState NewHandState);

	UFUNCTION()
	void OnHandItemChanged(const UCharacterInventoryComponent* Sender,
		const AActor* NewItem,
		const int32 NewHandItemIndex,
		const AActor* PreviousItem,
		const int32 PreviousHandItemIndex);

	UFUNCTION()
	void SwitchToNextItem_Internal() { SwitchToNextItem(); }

	UFUNCTION()
	void SwitchToPreviousItem_Internal() { SwitchToPreviousItem(); }

	UFUNCTION()
	void HandleHandStateChanged(const EItemState NewItemState, const AActor* Item, const int32 ItemIndex);

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void StopInteract();
};
