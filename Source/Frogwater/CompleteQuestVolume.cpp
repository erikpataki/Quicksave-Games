// Fill out your copyright notice in the Description page of Project Settings.


#include "CompleteQuestVolume.h"

#include "QuestDataAsset.h"
#include "QuestSubsystem.h"

void ACompleteQuestVolume::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(QuestToComplete), TEXT("Quest Volume '%s' does not have a valid Quest to complete."), *GetName());
}

void ACompleteQuestVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (OtherActor->IsA(InstigatorClass))
	{
		GetWorld()->GetSubsystem<UQuestSubsystem>()->CompleteQuest(QuestToComplete);
		Destroy();
	}
}
