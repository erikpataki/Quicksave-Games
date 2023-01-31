// Fill out your copyright notice in the Description page of Project Settings.


#include "FWCheatManager.h"

#include "QuestSubsystem.h"
#include "SpawnPoint.h"
#include "WorldStateSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"


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

void UFWCheatManager::AddQuest(const FString& QuestId)
{
	auto Quests = GetWorld()->GetSubsystem<UQuestSubsystem>();
	Quests->AddQuestById(FName(QuestId));
}

void UFWCheatManager::SetWorldCheckpoint(int32 NewCheckpoint, bool bTriggerIntermediateCheckpoints)
{
	auto WorldState = GetWorld()->GetSubsystem<UWorldStateSubsystem>();
	WorldState->SetWorldCheckpoint(NewCheckpoint, bTriggerIntermediateCheckpoints);
}

void UFWCheatManager::Spawn(const FString& ItemID)
{
	UE_LOG(LogTemp, Log, TEXT("Fabricate %s"), *ItemID);

	if (const auto AssetManager = UAssetManager::GetIfValid())
	{
		auto PrimaryId = FPrimaryAssetId("Item", FName(ItemID));
		AssetManager->LoadPrimaryAsset(PrimaryId, TArray<FName>(), FStreamableDelegate::CreateLambda([this, PrimaryId]()
		{
			if (const auto AssetManager = UAssetManager::GetIfValid())
			{
				auto ActorClass = AssetManager->GetPrimaryAssetObject<UClass>(PrimaryId);
				if (ActorClass)
				{
					if(const APlayerController* MyPlayerController = GetOuterAPlayerController())
					{
						const FRotator SpawnRot = MyPlayerController->GetControlRotation();
						FVector SpawnLoc = MyPlayerController->GetFocalLocation();

						SpawnLoc += 100.f * SpawnRot.Vector() + FVector(0.f, 0.f, 1.f) * 15.f;
						FActorSpawnParameters SpawnInfo;
						SpawnInfo.Instigator = MyPlayerController->GetInstigator();
						AActor* Actor = GetWorld()->SpawnActor(ActorClass, &SpawnLoc, &SpawnRot, SpawnInfo);
					}
				}
			}
		}));
	}
}
