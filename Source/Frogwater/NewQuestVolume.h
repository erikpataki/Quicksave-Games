// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "QuestDataAsset.h"
#include "Engine/TriggerVolume.h"
#include "NewQuestVolume.generated.h"

class UQuestDataAsset;
/**
 * Adds a new Quest to the game when the player walks through it.
 */
UCLASS()
class FROGWATER_API ANewQuestVolume : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Quests)
	const UQuestDataAsset* QuestToAdd = nullptr;

	// The type of Actor that can trigger this volume.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Quests)
	const TSubclassOf<AActor> InstigatorClass = AMainCharacter::StaticClass();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	UFUNCTION(BlueprintPure, Category=Quests)
	const UQuestDataAsset* GetQuestToAdd() const { return QuestToAdd; }
	
};
