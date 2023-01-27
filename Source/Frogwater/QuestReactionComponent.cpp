// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestReactionComponent.h"

#include "Quest.h"
#include "QuestSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "QuestDataAsset.h"

// Sets default values for this component's properties
UQuestReactionComponent::UQuestReactionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQuestReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	auto Quests = GetWorld()->GetSubsystem<UQuestSubsystem>();

	// Convert Quest Data Asset reference to Quest ID.
	if (!bUseQuestId)
	{
		if (!ensureMsgf(IsValid(RequiredQuest), TEXT("The Required Quest for Quest Reaction '%s' is invalid."), *GetOwner()->GetName()))
			return;

		RequiredQuestId = RequiredQuest->QuestId;
	}

	// No point subscribing if not a valid quest.
	if (!RequiredQuestId.IsNone())
	{
		if (RequiredProgress <= 0)
			Quests->OnQuestComplete.AddUniqueDynamic(this, &UQuestReactionComponent::OnQuestCompletion);
		else
			Quests->OnQuestUpdated.AddUniqueDynamic(this, &UQuestReactionComponent::OnQuestUpdated);
	}

	OnRequirementsMet.AddUniqueDynamic(this, &UQuestReactionComponent::RequirementsMet);

	// If no valid quest given, instantly invoke requirements met.
	if (RequiredQuestId.IsNone())
	{
		bRequirementsMet = true;
		OnRequirementsMet.Broadcast(this);
	}
}


// Called every frame
void UQuestReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UQuestReactionComponent::OnQuestCompletion(const FQuest& Quest)
{
	if (Quest.QuestId == RequiredQuestId)
	{
		bRequirementsMet = true;
		OnRequirementsMet.Broadcast(this);
		DestroyComponent();
	}
}

void UQuestReactionComponent::OnQuestUpdated(const FQuest& Quest, const float PreviousProgress,
	const float CurrentProgress)
{
	if (Quest.QuestId == RequiredQuestId && CurrentProgress >= RequiredProgress)
	{
		bRequirementsMet = true;
		OnRequirementsMet.Broadcast(this);
		DestroyComponent();
	}
}

void UQuestReactionComponent::RequirementsMet_Implementation(const UQuestReactionComponent* Self)
{
}

