// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScannableComponent.h"
#include "ScannableState.h"
#include "GameFramework/Actor.h"
#include "CommonScannableActor.generated.h"

UCLASS()
class FROGWATER_API ACommonScannableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACommonScannableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category="Camera")
	UScannableComponent* ScannableComponent = nullptr;

	UPROPERTY(EditAnywhere, Category="Camera")
	UStaticMeshComponent* StaticMeshComponent = nullptr;



public:
	UFUNCTION(BlueprintPure, Category="Camera")
	UScannableComponent* GetScannableComponent() const { return ScannableComponent; }

	UFUNCTION(BlueprintPure, Category="Camera")
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnPictureTaken(const UScannableComponent* Sender, const ACamera* Camera);

	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnCameraFocusing(const UScannableComponent* Sender, const ACamera* Camera);
	
	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnCameraFocus(const UScannableComponent* Sender, const ACamera* Camera);

	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnCameraOuterFocus(const UScannableComponent* Sender, const ACamera* Camera);

	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnCameraLostFocus(const UScannableComponent* Sender, const ACamera* Camera);
};
