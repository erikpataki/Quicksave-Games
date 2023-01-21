// Fill out your copyright notice in the Description page of Project Settings.


#include "NewQuestVolume.h"

#include "QuestSubsystem.h"

void ANewQuestVolume::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(IsValid(QuestToAdd), TEXT("Quest Volume '%s' does not have a valid Quest to add."), *GetName());
}

void ANewQuestVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor->IsA(InstigatorClass))
	{
		GetWorld()->GetSubsystem<UQuestSubsystem>()->AddQuest(QuestToAdd);
		Destroy();
	}
}
