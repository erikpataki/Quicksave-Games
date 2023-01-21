// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Item.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Camera.generated.h"

class UHermesInteractableComponent;
class UScannableComponent;
UCLASS()
class FROGWATER_API ACamera : public AActor, public IInteractable, public IItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual bool Interact_Implementation(AController* InInstigator) override;

	virtual void OnOffhand_Implementation(AController* InInstigator) override;
	virtual void OnHolstered_Implementation(AController* InInstigator) override;
	virtual void OnAim_Implementation(AController* InInstigator) override;
	
	virtual const FString GetItemName_Implementation() const override { return "Camera"; }
	virtual const UTexture2D* GetItemImage_Implementation() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Camera", BlueprintReadOnly)
	USceneCaptureComponent2D* SceneCaptureComponent;

	UPROPERTY(EditAnywhere, Category="Camera", BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="Camera", BlueprintReadOnly)
	UCapsuleComponent* PickupCollision;

	UPROPERTY(EditAnywhere, Category="Camera", BlueprintReadOnly)
	UHermesInteractableComponent* InteractableComponent;

protected:
	UPROPERTY(EditAnywhere, Category="Camera")
	FString ItemName = TEXT("Camera");
	
	UPROPERTY(EditAnywhere, Category="Camera")
	bool bEnablePinpointTrace = true;

	UPROPERTY(EditAnywhere, Category="Camera")
	float TraceMaxRadius = 100.f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float TraceMaxDistance = 400.f;

	UPROPERTY(EditAnywhere, Category="Camera")
	int32 TraceResolution = 3;

	UPROPERTY(EditAnywhere, Category="Camera", AdvancedDisplay)
	bool bUsePawnViewport = true;

	UPROPERTY(EditAnywhere, Category="Camera")
	float TakePictureCooldownTime = 3.f;

	FTimerHandle TakePictureCooldownTimer;

	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	UScannableComponent* FocusedScannable = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="Camera", AdvancedDisplay)
	APawn* OwningPawn = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	UTexture2D* LastPicture = nullptr;

	UPROPERTY(EditAnywhere, Category="Camera", AdvancedDisplay)
	float TraceTickInterval = 1 / 15.f;
	FTimerHandle TraceTimer;

public:
	UFUNCTION(BlueprintCallable, Category="Camera")
	void TakePicture();

	UFUNCTION(BlueprintPure, Category="Camera")
	APawn* GetOwningPawn() const { return OwningPawn; }

	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetOwningPawn(APawn* InOwningPawn);
	
	UFUNCTION(BlueprintPure, Category="Camera")
	UTexture2D* GetLastPicture() const { return LastPicture; }

	UFUNCTION(BlueprintPure, Category="Camera")
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

	UFUNCTION(BlueprintPure, Category="Camera")
	USceneCaptureComponent2D* GetSceneCaptureComponent() const { return SceneCaptureComponent; }

	UFUNCTION(BlueprintPure, Category="Camera")
	UCapsuleComponent* GetPickupCollisionComponent() const { return PickupCollision; }

	UFUNCTION(BlueprintCallable, Category="Camera")
	void SaveLastCaptureAsFile(FString FileNameWithoutExtension);

protected:
	void Trace();

	UFUNCTION(BlueprintPure, Category="Camera")
	FVector GetTraceOrigin() const;

	UFUNCTION(BlueprintPure, Category="Camera")
	FVector GetTraceEnd() const;

	UScannableComponent* TraceForBestScannable(float Radius);

	TArray<FHitResult> TraceForScanChannel(float Radius);

	TArray<FHitResult> TraceForVisibilityChannel(float Radius);

	UScannableComponent* GetHighestPriorityScannable(const TArray<FHitResult> HitResults) const;

	void RemoveNotVisibleScannables(TArray<FHitResult>& HitResults, const TArray<FHitResult>& VisibleHitResults) const;

	void MarkScannableComponent(UScannableComponent* ScannableComponent);

	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnPictureTaken(bool bSuccess, UScannableComponent* ScannableComponent);

	void StartTraceTimer();
	void StopTraceTimer();

	UFUNCTION()
	void OnTraceTick();

	UFUNCTION()
	void OnSceneCaptureTick();
};