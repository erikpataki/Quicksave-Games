// Fill out your copyright notice in the Description page of Project Settings.


#include "FWCheatManager.h"

#include "QuestSubsystem.h"
#include "SpawnPoint.h"
#include "WorldStateSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UFWCheatManager::TeleportTo(const FString& LocationName)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ASpawnPoint::StaticClass(), OUT FoundActors);

	for (const auto FoundActor : FoundActors)
	{
		if (const auto SpawnPoint = Cast<ASpawnPoint>(FoundActor))
		{
			if (SpawnPoint->GetLocationTag().ToUpper() == LocationName.ToUpper())
			{
				SpawnPoint->Spawn(GetPlayerController()->GetPawn());
				return;
			}
		}
	}
}

void UFWCheatManager::ProgressQuest(const FString& QuestId, float Progression)
{
	auto Quests = GetWorld()->GetSubsystem<UQuestSubsystem>();
	Quests->AddProgressToQuestById(FName(QuestId), Progression);
}

void UFWCheatManager::CompleteQuest(const FString& QuestId)
{
	auto Quests = GetWorld()->GetSubsystem<UQuestSubsystem>();
	Quests->CompleteQuestById(FName(QuestId));
}

void UFWCheatManager::SetWorldCheckpoint(int32 NewCheckpoint, bool bTriggerIntermediateCheckpoints)
{
	auto WorldState = GetWorld()->GetSubsystem<UWorldStateSubsystem>();
	WorldState->SetWorldCheckpoint(NewCheckpoint, bTriggerIntermediateCheckpoints);
}

void UFWCheatManager::SpawnCamera()
{
	Summon(TEXT("Game/Actors/Camera/BP_Camera"));
}
