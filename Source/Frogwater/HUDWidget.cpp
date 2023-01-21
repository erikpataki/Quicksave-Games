// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "FrogwaterHUD.h"

AFrogwaterHUD* UHUDWidget::GetHUD()
{
	if (!CachedHUD)
		if (const auto Player = GetOwningPlayer())
			CachedHUD = Player->GetHUD<AFrogwaterHUD>();
	
	return CachedHUD;
}

void UHUDWidget::OnQuestUpdated_Implementation(const FQuest& Quest, float PreviousProgress, float CurrentProgress)
{
}

void UHUDWidget::OnQuestComplete_Implementation(const FQuest& Quest)
{
}

void UHUDWidget::OnNewQuest_Implementation(const FQuest& Quest)
{
}
