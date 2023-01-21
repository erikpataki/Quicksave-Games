// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class AFrogwaterHUD;
/**
 * 
 */
UCLASS()
class FROGWATER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	AFrogwaterHUD* CachedHUD;

public:
	UFUNCTION(BlueprintPure, Category="HUD")
	AFrogwaterHUD* GetHUD();

	UFUNCTION(BlueprintNativeEvent, Category="HUD|Quests")
	void OnNewQuest(const FQuest& Quest);
	
	UFUNCTION(BlueprintNativeEvent, Category="HUD|Quests")
	void OnQuestComplete(const FQuest& Quest);

	UFUNCTION(BlueprintNativeEvent, Category="HUD|Quests")
	void OnQuestUpdated(const FQuest& Quest, float PreviousProgress, float CurrentProgress);
	
};
