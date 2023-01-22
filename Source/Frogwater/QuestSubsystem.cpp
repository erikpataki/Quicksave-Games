// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSubsystem.h"

#include "QuestDataAsset.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UQuestSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

FQuest UQuestSubsystem::GetQuestById(FName QuestId)
{
	if (const auto Quest = GetQuestByPtr(QuestId))
		return *Quest;

	return FQuest();
}

FQuest UQuestSubsystem::GetQuest(const UQuestDataAsset* QuestDataAsset)
{
	if (IsValid(QuestDataAsset))
		return GetQuestById(QuestDataAsset->QuestId);

	return FQuest();
}

bool UQuestSubsystem::IsQuestCompleteById(FName QuestId)
{
	if (const auto Quest = GetQuestByPtr(QuestId))
		return Quest->bComplete;

	return false;
}

bool UQuestSubsystem::IsQuestComplete(const UQuestDataAsset* QuestDataAsset)
{
	if (IsValid(QuestDataAsset))
		return IsQuestCompleteById(QuestDataAsset->QuestId);

	return false;
}

bool UQuestSubsystem::IsQuestActiveById(FName QuestId)
{
	if (const auto Quest = GetQuestByPtr(QuestId))
		return !Quest->bComplete;

	return false;
}

bool UQuestSubsystem::IsQuestActive(const UQuestDataAsset* QuestDataAsset)
{
	if (IsValid(QuestDataAsset))
		return IsQuestActiveById(QuestDataAsset->QuestId);

	return false;
}

FQuest* UQuestSubsystem::GetQuestByPtr(FName QuestId)
{
	if (FQuest* Quest = MainQuests.Find(QuestId))
		return &*Quest;
	if (FQuest* Quest = SideQuests.Find(QuestId))
		return &*Quest;
	if (FQuest* Quest = SecretQuests.Find(QuestId))
		return &*Quest;
	
	return nullptr;
}

void UQuestSubsystem::AddQuest(const UQuestDataAsset* Quest)
{
	if (!IsValid(Quest))
		return;

	FQuest QuestData;
	QuestData.QuestId = Quest->QuestId;
	QuestData.QuestName = Quest->QuestName;
	QuestData.QuestDescription = Quest->QuestDescription;
	QuestData.QuestType = Quest->QuestType;
	QuestData.RequiredProgress = Quest->RequiredProgress;
	QuestData.bHasPicture = Quest->bHasPicture;
	QuestData.QuestToGiveUponCompletion = Quest->QuestToGiveUponCompletion;

	switch (QuestData.QuestType)
	{
		case EQuestType::MainQuest:
			// Quest already exists.
			if (!ensureAlwaysMsgf(!MainQuests.Contains(QuestData.QuestId),
					TEXT("Quest '%s' already exists"), *QuestData.QuestId.ToString()))
			{
				return;
			}
		
			MainQuests.Add(QuestData.QuestId, QuestData);
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(TEXT("Main Quest Added: '%s' (id: %s)"), *QuestData.QuestName, *QuestData.QuestId.ToString()));
			break;
		case EQuestType::SideQuest:
			// Quest already exists.
			if (!ensureAlwaysMsgf(!MainQuests.Contains(QuestData.QuestId),
					TEXT("Quest '%s' already exists"), *QuestData.QuestId.ToString()))
			{
				return;
			}
		
			SideQuests.Add(QuestData.QuestId, QuestData);
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(TEXT("Side Quest Added: '%s' (id: %s)"), *QuestData.QuestName, *QuestData.QuestId.ToString()));
	
			break;
		case EQuestType::SecretQuest:
			// Quest already exists.
			if (!ensureAlwaysMsgf(!MainQuests.Contains(QuestData.QuestId),
					TEXT("Quest '%s' already exists"), *QuestData.QuestId.ToString()))
			{
				return;
			}
		
			SecretQuests.Add(QuestData.QuestId, QuestData);
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(TEXT("Secret Quest Added: '%s' (id: %s)"), *QuestData.QuestName, *QuestData.QuestId.ToString()));
	
			break;
	}

	OnQuestAdded.Broadcast(QuestData);
}

bool UQuestSubsystem::AddProgressToQuestById(FName QuestId, float ScoreToAdd)
{
	if (const auto QuestPtr = GetQuestByPtr(QuestId))
	{
		// Quest already complete.
		if (!ensureAlwaysMsgf(!QuestPtr->bComplete,
				TEXT("Progress was added to Quest '%s' but it is already complete"), *QuestId.ToString()))
		{
			return false;
		}
		
		const float PreviousProgress = QuestPtr->CurrentProgress;
		
		if ((QuestPtr->CurrentProgress += ScoreToAdd) >= QuestPtr->RequiredProgress)
			ProcessQuestCompleted(*QuestPtr);
		else
		{
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(TEXT("Quest Updated from %f -> %f/%f: '%s' (id: %s)"),
					PreviousProgress, QuestPtr->CurrentProgress, QuestPtr->RequiredProgress,
					*QuestPtr->QuestName, *QuestPtr->QuestId.ToString()));
	
			OnQuestUpdated.Broadcast(*QuestPtr, PreviousProgress, QuestPtr->CurrentProgress);
		}

		return true;
	}

	return false;
}

bool UQuestSubsystem::AddProgressToQuest(const UQuestDataAsset* QuestDataAsset, float ScoreToAdd)
{
	return IsValid(QuestDataAsset) ? AddProgressToQuestById(QuestDataAsset->QuestId, ScoreToAdd) : false;
}

bool UQuestSubsystem::CompleteQuestById(FName QuestId)
{
	return AddProgressToQuestById(QuestId, FLT_MAX);
}

bool UQuestSubsystem::CompleteQuest(const UQuestDataAsset* QuestDataAsset)
{
	return IsValid(QuestDataAsset) ? CompleteQuestById(QuestDataAsset->QuestId) : false;
}

void UQuestSubsystem::ProcessQuestCompleted(FQuest& Quest)
{
	UKismetSystemLibrary::PrintString(
		this,
		FString::Printf(TEXT("Quest Complete: '%s' (id: %s)"), *Quest.QuestName, *Quest.QuestId.ToString()));
	
	Quest.bComplete = true;
	OnQuestComplete.Broadcast(Quest);

	if (IsValid(Quest.QuestToGiveUponCompletion))
		AddQuest(Quest.QuestToGiveUponCompletion);
}

UTexture2D* UQuestSubsystem::GetPictureFromQuest(const UQuestDataAsset* QuestDataAsset)
{
	const FString CompleteFileName = GetPictureFilenameForQuest(QuestDataAsset);
	if (!CompleteFileName.IsEmpty())
		return UKismetRenderingLibrary::ImportFileAsTexture2D(this, CompleteFileName);

	return nullptr;
}

FString UQuestSubsystem::GetPictureFilenameForQuest(const UQuestDataAsset* QuestDataAsset) const
{
	// Invalid Quest or Quest doesn't require a picture.
	if (!IsValid(QuestDataAsset) || !QuestDataAsset->bHasPicture)
		return FString();

	const FString CompleteFileName =
		FPaths::Combine(FPaths::ProjectUserDir(), TEXT("Pictures"), QuestDataAsset->QuestId.ToString() + TEXT(".HDR"));
	return CompleteFileName;
}

const TMap<FName, FQuest> UQuestSubsystem::GetCompletedMainQuests() const
{
	TMap<FName, FQuest> CompletedQuests;

	for (auto& Quest : GetMainQuests())
		if (Quest.Value.bComplete)
			CompletedQuests.Add(Quest);

	return CompletedQuests;
}

const TMap<FName, FQuest> UQuestSubsystem::GetCompletedSideQuests() const
{
	TMap<FName, FQuest> CompletedQuests;

	for (auto& Quest : GetSideQuests())
		if (Quest.Value.bComplete)
			CompletedQuests.Add(Quest);

	return CompletedQuests;
}

const TMap<FName, FQuest> UQuestSubsystem::GetCompletedSecretQuests() const
{
	TMap<FName, FQuest> CompletedQuests;

	for (auto& Quest : GetSecretQuests())
		if (Quest.Value.bComplete)
			CompletedQuests.Add(Quest);

	return CompletedQuests;
}

const TMap<FName, const FQuest*> UQuestSubsystem::GetCompletedMainQuestsByPtr() const
{
	TMap<FName, const FQuest*> CompletedQuests;

	for (auto& Quest : GetMainQuests())
		if (Quest.Value.bComplete)
			CompletedQuests.Add(Quest.Key, &Quest.Value);

	return CompletedQuests;
}

const TMap<FName, const FQuest*> UQuestSubsystem::GetCompletedSideQuestsByPtr() const
{
	TMap<FName, const FQuest*> CompletedQuests;

	for (auto& Quest : GetSideQuests())
		if (Quest.Value.bComplete)
			CompletedQuests.Add(Quest.Key, &Quest.Value);

	return CompletedQuests;
}

const TMap<FName, const FQuest*> UQuestSubsystem::GetCompletedSecretQuestsByPtr() const
{
	TMap<FName, const FQuest*> CompletedQuests;

	for (auto& Quest : GetSecretQuests())
		if (Quest.Value.bComplete)
			CompletedQuests.Add(Quest.Key, &Quest.Value);

	return CompletedQuests;
}
