// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogwaterHUD.h"

#include "QuestSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"

void AFrogwaterHUD::BeginPlay()
{
	Super::BeginPlay();

	SetupQuestsLink();

	/*auto PlayerChar = GetOwningPlayerController()->GetPawn<AMainCharacter>();
	if (ensureAlwaysMsgf(IsValid(PlayerChar), TEXT("The HUD could not find an owning Character")))
	{
		PlayerChar->OnHandStateChanged.AddUniqueDynamic(this, );
		
		if (auto PlayerInventory = Cast<UCharacterInventoryComponent>(
			PlayerChar->GetComponentByClass(UCharacterInventoryComponent::StaticClass())); IsValid(PlayerInventory))
		{
			PlayerInventory->OnHandItemChanged.AddUniqueDynamic(this,);
			PlayerInventory->OnItemAdded.AddUniqueDynamic(this, );
			PlayerInventory->OnItemRemoved.AddUniqueDynamic(this, );
		}
	}*/

	OverlayHUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), OverlayHUDWidgetClass, FName(TEXT("OverlayHUD")));
	if (OverlayHUDWidget)
		OverlayHUDWidget->AddToPlayerScreen(999);

	SwitchHUDState(EHUDState::Normal);
}

void AFrogwaterHUD::SwitchHUDState(EHUDState NewHUDState)
{
	if (NewHUDState == HUDState)
		return;

	HUDState = NewHUDState;
	
	switch (NewHUDState)
	{
	case EHUDState::None:
		bShowHUD = false;
		break;
		case EHUDState::Minimal:
			if (IsValid(NormalHUDWidget))
				NormalHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(CameraHUDWidget))
				CameraHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		
			if (!IsValid(MinimalHUDWidget))
			{
				MinimalHUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), MinimalHUDWidgetClass, FName(TEXT("MinimalHUD")));
				if (MinimalHUDWidget)
					MinimalHUDWidget->AddToPlayerScreen();
			}

			MinimalHUDWidget->SetVisibility(ESlateVisibility::Visible);

			bShowHUD = true;
		
			OnSwitchedToMinimalHUD();
			break;
		case EHUDState::Normal:
			if (IsValid(MinimalHUDWidget))
				MinimalHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(CameraHUDWidget))
				CameraHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
			
			if (!IsValid(NormalHUDWidget))
			{
				NormalHUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), NormalHUDWidgetClass, FName(TEXT("NormalHUD")));
				if (NormalHUDWidget)
					NormalHUDWidget->AddToPlayerScreen();
			}

			NormalHUDWidget->SetVisibility(ESlateVisibility::Visible);

			bShowHUD = true;
		
			OnSwitchedToNormalHUD();
			break;
		case EHUDState::PolaroidCamera:
			if (IsValid(MinimalHUDWidget))
				MinimalHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(NormalHUDWidget))
				NormalHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
				
			if (!IsValid(CameraHUDWidget))
			{
				CameraHUDWidget = CreateWidget<UHUDWidget>(GetOwningPlayerController(), CameraHUDWidgetClass, FName(TEXT("CameraHUD")));
				if (CameraHUDWidget)
					CameraHUDWidget->AddToPlayerScreen();
			}

			CameraHUDWidget->SetVisibility(ESlateVisibility::Visible);

			bShowHUD = true;
		
			OnSwitchedToCameraHUD();
			break;
	}
}

void AFrogwaterHUD::OnSwitchedToCameraHUD_Implementation()
{
}

void AFrogwaterHUD::OnSwitchedToNormalHUD_Implementation()
{
}

void AFrogwaterHUD::OnSwitchedToMinimalHUD_Implementation()
{
}

void AFrogwaterHUD::SetupQuestsLink()
{
	const auto Quests = GetWorld()->GetSubsystem<UQuestSubsystem>();
	Quests->OnQuestAdded.AddUniqueDynamic(this, &AFrogwaterHUD::AFrogwaterHUD::OnQuestAdded);
	Quests->OnQuestComplete.AddUniqueDynamic(this, &AFrogwaterHUD::OnQuestCompleted);
	Quests->OnQuestUpdated.AddUniqueDynamic(this, &AFrogwaterHUD::OnQuestUpdated);

	for (auto& Quest : Quests->GetMainQuests())
		if (!Quest.Value.bComplete)
			CachedInProgressMainQuests.Add(Quest.Key, Quest.Value);

	for (auto& Quest : Quests->GetSideQuests())
		if (!Quest.Value.bComplete)
			CachedInProgressSideQuests.Add(Quest.Key, Quest.Value);
}

void AFrogwaterHUD::OnQuestAdded(const FQuest& Quest)
{
	if (Quest.QuestType == EQuestType::MainQuest)
		CachedInProgressMainQuests.Add(Quest.QuestId, Quest);
	else if (Quest.QuestType == EQuestType::SideQuest)
		CachedInProgressSideQuests.Add(Quest.QuestId, Quest);

	if (IsValid(OverlayHUDWidget))
		OverlayHUDWidget->OnNewQuest(Quest);
	if (IsValid(GetActiveHUDWidget()))
		GetActiveHUDWidget()->OnNewQuest(Quest);
}

void AFrogwaterHUD::OnQuestUpdated(const FQuest& Quest, const float PreviousProgress, const float CurrentProgress)
{
	if (IsValid(OverlayHUDWidget))
		OverlayHUDWidget->OnQuestUpdated(Quest, PreviousProgress, CurrentProgress);
	if (IsValid(GetActiveHUDWidget()))
		GetActiveHUDWidget()->OnQuestUpdated(Quest, PreviousProgress, CurrentProgress);
}

void AFrogwaterHUD::OnQuestCompleted(const FQuest& Quest)
{
	if (Quest.QuestType == EQuestType::MainQuest)
		CachedInProgressMainQuests.Remove(Quest.QuestId);
	else if (Quest.QuestType == EQuestType::SideQuest)
		CachedInProgressSideQuests.Remove(Quest.QuestId);

	if (IsValid(OverlayHUDWidget))
		OverlayHUDWidget->OnQuestComplete(Quest);
	if (IsValid(GetActiveHUDWidget()))
		GetActiveHUDWidget()->OnQuestComplete(Quest);
}

UHUDWidget* AFrogwaterHUD::GetActiveHUDWidget() const
{
	switch (HUDState)
	{
		case EHUDState::None:
			return nullptr;
		case EHUDState::Minimal:
			return MinimalHUDWidget;
		case EHUDState::Normal:
			return NormalHUDWidget;
		case EHUDState::PolaroidCamera:
			return CameraHUDWidget;
		default:
			return nullptr;
	}
}
