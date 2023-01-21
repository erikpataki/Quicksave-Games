// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Components/ActorComponent.h"
#include "QuestReactionComponent.generated.h"


class UQuestDataAsset;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class FROGWATER_API UQuestReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestReactionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, Category=Quest)
	bool bUseQuestId = false;
	
	UPROPERTY(EditAnywhere, Category=Quest, meta=(EditCondition="bUseQuestId"))
	FName RequiredQuestId;

	UPROPERTY(EditAnywhere, Category=Quest, meta=(EditCondition="!bUseQuestId"))
	const UQuestDataAsset* RequiredQuest;

	UPROPERTY(EditAnywhere, Category=Quest)
	float RequiredProgress = 0.f;

	UPROPERTY(VisibleAnywhere, Category=Quest)
	bool bRequirementsMet = false;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestRequirementsSignature,
		const UQuestReactionComponent*, Sender);

	UPROPERTY(BlueprintAssignable, Category=Quest)
	FQuestRequirementsSignature OnRequirementsMet;

protected:
	UFUNCTION()
	void OnQuestCompletion(const FQuest& Quest);

	UFUNCTION()
	void OnQuestUpdated(const FQuest& Quest, const float PreviousProgress, const float CurrentProgress);

	UFUNCTION(BlueprintNativeEvent, Category=Quest)
	void RequirementsMet(const UQuestReactionComponent* Sender);
};
