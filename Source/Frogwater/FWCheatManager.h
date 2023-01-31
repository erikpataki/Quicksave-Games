// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "FWCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API UFWCheatManager : public UCheatManager
{
	GENERATED_BODY()

	UFUNCTION(exec,BlueprintCallable,Category="Cheat Manager")
	void TeleportTo(const FString& LocationName);

	UFUNCTION(exec,BlueprintCallable,Category="Cheat Manager")
	void ProgressQuest(const FString& QuestId, float Progression = 1.f);

	UFUNCTION(exec,BlueprintCallable,Category="Cheat Manager")
	void CompleteQuest(const FString& QuestId);

	UFUNCTION(exec,BlueprintCallable,Category="Cheat Manager")
	void SetWorldCheckpoint(int32 NewCheckpoint, bool bTriggerIntermediateCheckpoints = false);

	UFUNCTION(exec,BlueprintCallable,Category="Cheat Manager")
	void SpawnCamera();
};
