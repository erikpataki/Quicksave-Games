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
	virtual void Turn(float Rate) override;
	virtual void LookUp(float Rate) override;

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

	UPROPERTY(EditAnywhere, Category=Camera)
	float ZoomStep = .25f;

	UPROPERTY(EditAnywhere, Category=Camera)
	float DefaultFov = 90.f;

	UPROPERTY(EditAnywhere, Category=Camera)
	float TargetFov = DefaultFov;

	UPROPERTY(EditAnywhere, Category=Camera)
	float FovChangeSpeed = 10.f;

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

	UFUNCTION(BlueprintCallable, Category=Camera)
	void SetTargetFov(float NewFov, bool bForceChange = false);

	UFUNCTION(BlueprintCallable, Category=Camera)
	void ResetTargetFov(bool bForceChange = false) { SetTargetFov(GetDefaultFov(), bForceChange); }

	UFUNCTION(BlueprintPure, Category=Camera)
	float GetDefaultFov() const { return DefaultFov; }

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

	UFUNCTION(BlueprintNativeEvent, Category=Interaction)
	void ZoomIn();

	UFUNCTION(BlueprintNativeEvent, Category=Interaction)
	void ZoomOut();

	
// Hand sway
protected:
	UPROPERTY(EditAnywhere, Category="Hand Sway")
	float HandSwayAimReduction = .25f;
	
	UPROPERTY(EditAnywhere, Category="Hand Sway")
	FRotator MaxHandSway = FRotator(7.5, 7.5, 5);

	UPROPERTY(EditAnywhere, Category="Hand Sway")
	FRotator MinHandSway = FRotator(-7.5, -7.5, -5);

	UPROPERTY(EditAnywhere, Category="Hand Sway")
	float HandSwaySpeed = 5.f;

	UPROPERTY(EditAnywhere, Category="Hand Sway")
	FRotator HandSwayMultiplier = FRotator(1.f, 1.f, .5f);
	
	// Should Hands sway naturally.
	UPROPERTY(EditAnywhere, Category="Hand Sway|Breathing")
	bool bEnableBreathingHandSway = false;
	
	UPROPERTY(EditAnywhere, Category="Hand Sway|Breathing")
	float BreathingHandSwayMultiplier;

	// Should Character rotation changes cause Hand Sway?
	UPROPERTY(EditAnywhere, Category="Hand Sway|Rotation")
	bool bEnableRotationHandSway = true;

	UPROPERTY(EditAnywhere, Category="Hand Sway|Rotation")
	float RotationHandSwayMultiplier = .5f;

	// Should Character location changes cause Hand Sway?
	UPROPERTY(EditAnywhere, Category="Hand Sway|Location")
	bool bEnableLocationHandSway = true;

	UPROPERTY(EditAnywhere, Category="Hand Sway|Location")
	float LocationHandSwayMultiplier = .0067f;

protected:
	void UpdateHandSway(float DeltaTime);
	void ApplyBreathingHandSway(FRotator& TargetRotation);
	void ApplyRotationHandSway(FRotator& TargetRotation);
	void ApplyLocationHandSway(FRotator& TargetRotation);
	void ApplyHandSwayRotation(FRotator& TargetRotation, float Pitch, float Yaw);
};
