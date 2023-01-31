#pragma once
#include "QuestType.h"
#include "QuestDataAsset.generated.h"

UCLASS(BlueprintType, Blueprintable)
class FROGWATER_API UQuestDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Quest)
	FName QuestId = TEXT("quest_");
	
	UPROPERTY(EditAnywhere, Category=Quest)
	FString QuestName = TEXT("Hello");

	UPROPERTY(EditAnywhere, Category=Quest)
	FString QuestDescription = TEXT("Kill Grandma!");

	UPROPERTY(EditAnywhere, Category=Quest)
	EQuestType QuestType = EQuestType::MainQuest;

	UPROPERTY(EditAnywhere, Category=Quest)
	float RequiredProgress = 1.f;

	UPROPERTY(EditAnywhere, Category=Quest)
	bool bHasPicture = false;
	
	UPROPERTY(EditAnywhere, Category=Quest)
	class UQuestDataAsset* QuestToGiveUponCompletion;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

inline FPrimaryAssetId UQuestDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("Quest", QuestId);
}
