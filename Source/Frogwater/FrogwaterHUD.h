// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDState.h"
#include "HUDWidget.h"
#include "Quest.h"
#include "GameFramework/HUD.h"
#include "FrogwaterHUD.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API AFrogwaterHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleInstanceOnly, Category=HUD)
	EHUDState HUDState;

	UPROPERTY(EditDefaultsOnly, Category=HUD)
	TSubclassOf<UHUDWidget> MinimalHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category=HUD)
	TSubclassOf<UHUDWidget> NormalHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category=HUD)
	TSubclassOf<UHUDWidget> CameraHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category=HUD)
	TSubclassOf<UHUDWidget> OverlayHUDWidgetClass;
	

	UPROPERTY()
	UHUDWidget* MinimalHUDWidget;

	UPROPERTY()
	UHUDWidget* NormalHUDWidget;

	UPROPERTY()
	UHUDWidget* CameraHUDWidget;

	UPROPERTY()
	UHUDWidget* OverlayHUDWidget;
	
public:
	UFUNCTION(BlueprintCallable, Category=HUD)
	void SwitchHUDState(EHUDState NewHUDState);

	UFUNCTION(BlueprintNativeEvent, Category=HUD)
	void OnSwitchedToMinimalHUD();

	UFUNCTION(BlueprintNativeEvent, Category=HUD)
	void OnSwitchedToNormalHUD();

	UFUNCTION(BlueprintNativeEvent, Category=HUD)
	void OnSwitchedToCameraHUD();

// QUESTS LINK
protected:
	UPROPERTY(BlueprintReadOnly, Category="HUD|Quests")
	TMap<FName, FQuest> CachedInProgressMainQuests;

	UPROPERTY(BlueprintReadOnly, Category="HUD|Quests")
	TMap<FName, FQuest> CachedInProgressSideQuests;
	
public:
	UFUNCTION(BlueprintPure, Category="HUD|Quests")
	const TMap<FName, FQuest>& GetInProgressMainQuests() const { return CachedInProgressMainQuests; }

	UFUNCTION(BlueprintPure, Category="HUD|Quests")
	const TMap<FName, FQuest>& GetInProgressSideQuests() const { return CachedInProgressSideQuests; }
	
protected:
	void SetupQuestsLink();
	
	UFUNCTION()
	void OnQuestAdded(const FQuest& Quest);

	UFUNCTION()
	void OnQuestUpdated(const FQuest& Quest, const float PreviousProgress, const float CurrentProgress);

	UFUNCTION()
	void OnQuestCompleted(const FQuest& Quest);

	UFUNCTION(BlueprintPure, Category="HUD|Quests")
	UHUDWidget* GetActiveHUDWidget() const;
};
