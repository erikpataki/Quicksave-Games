// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldCheckpointTriggerVolume.h"

#include "QuestReactionComponent.h"
#include "WorldStateSubsystem.h"

AWorldCheckpointTriggerVolume::AWorldCheckpointTriggerVolume()
{
	QuestReactionComponent = CreateDefaultSubobject<UQuestReactionComponent>(TEXT("Quest Reaction"));
}

void AWorldCheckpointTriggerVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(GetWorld()) && GetWorld()->IsGameWorld())
	{
		SetActorEnableCollision(false);
		QuestReactionComponent->OnRequirementsMet.AddUniqueDynamic(this, &AWorldCheckpointTriggerVolume::HandleQuestRequirementsMet);
	}
}

void AWorldCheckpointTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
}

void AWorldCheckpointTriggerVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	GetWorld()->GetSubsystem<UWorldStateSubsystem>()->SetWorldCheckpoint(GetWorldCheckpoint(), bDontSkipWorldCheckpoints);
	Destroy();
}

void AWorldCheckpointTriggerVolume::HandleQuestRequirementsMet(const UQuestReactionComponent* Sender)
{
	SetActorEnableCollision(true);
}
