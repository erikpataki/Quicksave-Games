// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "Engine/TriggerVolume.h"
#include "WorldCheckpointTriggerVolume.generated.h"

class UQuestReactionComponent;
/**
 * 
 */
UCLASS()
class FROGWATER_API AWorldCheckpointTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	AWorldCheckpointTriggerVolume();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	UQuestReactionComponent* QuestReactionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	int32 WorldCheckpoint = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	bool bDontSkipWorldCheckpoints = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	const TSubclassOf<AActor> InstigatorClass = AMainCharacter::StaticClass();

public:
	UFUNCTION(BlueprintPure, Category="World State")
	int32 GetWorldCheckpoint() const { return WorldCheckpoint; }

protected:
	UFUNCTION()
	void HandleQuestRequirementsMet(const UQuestReactionComponent* Sender);
};