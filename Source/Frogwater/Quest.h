#pragma once
#include "QuestType.h"
#include "Quest.generated.h"

USTRUCT(BlueprintType)
struct FROGWATER_API FQuest
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	FName QuestId = TEXT("quest_");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	FString QuestName = TEXT("Hello");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	FString QuestDescription = TEXT("Kill Grandma!");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	EQuestType QuestType = EQuestType::MainQuest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	float RequiredProgress = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	float CurrentProgress;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	bool bComplete = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Quest)
	bool bHasPicture = false;
};
