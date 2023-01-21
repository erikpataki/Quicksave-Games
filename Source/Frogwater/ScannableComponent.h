// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera.h"
#include "ScannableState.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "ScannableComponent.generated.h"

#define SCANNABLE_CHANNEL		ECollisionChannel::ECC_GameTraceChannel2


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROGWATER_API UScannableComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UScannableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	bool bPictureTaken = false;

	UPROPERTY(EditAnywhere, Category="Camera")
	int32 Priority = 10;

	UPROPERTY(EditAnywhere, Category="Camera")
	bool bEnableAtStart = false;

	UPROPERTY(EditAnywhere, Category="Camera")
	float TimeTillFocused = 1.f;
	
	UPROPERTY(EditAnywhere, Category="Camera")
	float TimeTillUnfocused = .5f;

	UPROPERTY(EditAnywhere, Category="Camera")
	bool bCompleteQuestUponPictureTaken = false;

	UPROPERTY(EditAnywhere, Category="Camera", meta=(EditCondition="bCompleteQuestUponPictureTaken"))
	FName QuestName;

	UPROPERTY(EditAnywhere, Category="Camera", meta=(EditCondition="bCompleteQuestUponPictureTaken"))
	bool bSavePictureToQuest = false;

	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	EScannableState State;

	UPROPERTY(VisibleInstanceOnly, Category="Camera")
	FDateTime LastFocusTime;

	float TimeFocused;
	float TimeUnfocused;

public:
	UFUNCTION(BlueprintPure, Category="Camera")
	int32 GetPriority() const { return Priority; }

	UFUNCTION(BlueprintCallable, Category="Camera")
	void Enable();

	UFUNCTION(BlueprintCallable, Category="Camera")
	float GetTimeFocused() const { return TimeFocused; }

	UFUNCTION(BlueprintCallable, Category="Camera")
	float GetTimeUnfocused() const { return TimeUnfocused; }
	
	bool ExecPictureTaken(ACamera* Camera);

	void ExecIsFocus(ACamera* Camera);

	void ExecIsOuterFocus(ACamera* Camera);


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPictureEventSignature,
		const UScannableComponent*, Sender,
		const ACamera*, Camera);

	UPROPERTY(BlueprintAssignable, Category="Camera")
	FPictureEventSignature OnPictureTaken;

	UPROPERTY(BlueprintAssignable, Category="Camera")
	FPictureEventSignature OnCameraFocus;

	UPROPERTY(BlueprintAssignable, Category="Camera")
	FPictureEventSignature OnCameraOuterFocus;
	
	UPROPERTY(BlueprintAssignable, Category="Camera")
	FPictureEventSignature OnCameraLostFocus;
};
