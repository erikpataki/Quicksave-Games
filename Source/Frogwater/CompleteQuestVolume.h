// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "Engine/TriggerVolume.h"
#include "CompleteQuestVolume.generated.h"

class UQuestDataAsset;
/**
 * 
 */
UCLASS()
class FROGWATER_API ACompleteQuestVolume : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Quests)
	const UQuestDataAsset* QuestToComplete = nullptr;

	// The type of Actor that can trigger this volume.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Quests)
	const TSubclassOf<AActor> InstigatorClass = AMainCharacter::StaticClass();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	UFUNCTION(BlueprintPure, Category=Quests)
	const UQuestDataAsset* GetQuestToComplete() const { return QuestToComplete; }
};
