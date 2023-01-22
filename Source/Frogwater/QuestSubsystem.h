// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuestSubsystem.generated.h"

class UQuestDataAsset;
/**
 * 
 */
UCLASS()
class FROGWATER_API UQuestSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

protected:
	UPROPERTY()
	TMap<FName, FQuest> MainQuests;

	UPROPERTY()
	TMap<FName, FQuest> SideQuests;

	UPROPERTY()
	TMap<FName, FQuest> SecretQuests;

public:
	UFUNCTION(BlueprintPure, Category=Quest)
	const TMap<FName, FQuest>& GetMainQuests() const { return MainQuests; }

	UFUNCTION(BlueprintPure, Category=Quest)
	const TMap<FName, FQuest>& GetSideQuests() const { return MainQuests; }

	UFUNCTION(BlueprintPure, Category=Quest)
	const TMap<FName, FQuest>& GetSecretQuests() const { return MainQuests; }

	UFUNCTION(BlueprintPure, Category=Quest)
	FQuest GetQuestById(FName QuestId);
	
	UFUNCTION(BlueprintPure, Category=Quest)
	FQuest GetQuest(const UQuestDataAsset* QuestDataAsset);

	UFUNCTION(BlueprintPure, Category=Quest)
	bool IsQuestCompleteById(FName QuestId);
	
	UFUNCTION(BlueprintPure, Category=Quest)
	bool IsQuestComplete(const UQuestDataAsset* QuestDataAsset);

	UFUNCTION(BlueprintPure, Category=Quest)
	bool IsQuestActiveById(FName QuestId);

	UFUNCTION(BlueprintPure, Category=Quest)
	bool IsQuestActive(const UQuestDataAsset* QuestDataAsset);

	FQuest* GetQuestByPtr(FName QuestId);

	UFUNCTION(BlueprintCallable, Category=Quest)
	void AddQuest(const UQuestDataAsset* Quest);

	UFUNCTION(BlueprintCallable, Category=Quest)
	bool AddProgressToQuestById(FName QuestId, float ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category=Quest)
	bool AddProgressToQuest(const UQuestDataAsset* QuestDataAsset, float ScoreToAdd);

	UFUNCTION(BlueprintCallable, Category=Quest)
	bool CompleteQuestById(FName QuestId);

	UFUNCTION(BlueprintCallable, Category=Quest)
	bool CompleteQuest(const UQuestDataAsset* QuestDataAsset);

	void ProcessQuestCompleted(FQuest& Quest);

	// Not async. Try to use the 'Load Quest Picture' BP node instead.
	UFUNCTION(BlueprintPure, Category=Quest)
	UTexture2D* GetPictureFromQuest(const UQuestDataAsset* QuestDataAsset);

	UFUNCTION(BlueprintPure, Category=Quest)
	FString GetPictureFilenameForQuest(const UQuestDataAsset* QuestDataAsset) const;

	UFUNCTION(BlueprintPure, Category=Quest)
	const TMap<FName, FQuest> GetCompletedMainQuests() const;

	UFUNCTION(BlueprintPure, Category=Quest)
	const TMap<FName, FQuest> GetCompletedSideQuests() const;

	UFUNCTION(BlueprintPure, Category=Quest)
	const TMap<FName, FQuest> GetCompletedSecretQuests() const;

	const TMap<FName, const FQuest*> GetCompletedMainQuestsByPtr() const;
	const TMap<FName, const FQuest*> GetCompletedSideQuestsByPtr() const;
	const TMap<FName, const FQuest*> GetCompletedSecretQuestsByPtr() const;
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestCompleteSignature,
		const FQuest&, Quest);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FQuestUpdatedSignature,
		const FQuest&, Quest,
		const float, PreviousProgress,
		const float, CurrentProgress);

	UPROPERTY(BlueprintAssignable, Category=Quest)
	FQuestCompleteSignature OnQuestComplete;

	UPROPERTY(BlueprintAssignable, Category=Quest)
	FQuestUpdatedSignature OnQuestUpdated;

	UPROPERTY(BlueprintAssignable, Category=Quest)
	FQuestCompleteSignature OnQuestAdded;
};
