// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStateSubsystem.h"

#include "WorldChangeDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"

void UWorldStateSubsystem::PostInitialize()
{
	Super::PostInitialize();
	
	FString DTPath = TEXT("DataTable'/Game/WorldState/DT_WorldCheckpoints.DT_WorldCheckpoints'");

	const UDataTable* DT = (
		LoadObject<UDataTable>(nullptr, *DTPath));

	checkf(DT, TEXT("Could not find World Checkpoints DataTable at %s"), *DTPath);

	WorldCheckpoints.Reserve(DT->GetRowMap().Num());
	DT->ForeachRow<FWorldCheckpoint>(TEXT("WorldState"), [this](
		const FName& Key, const FWorldCheckpoint& Value)
	{
		// Add by pointer coz its statically stored in Data Table anyways.
		WorldCheckpoints.Emplace(&Value);
	});
}

void UWorldStateSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

}

bool UWorldStateSubsystem::IsWorldChangeActive(UWorldChangeDataAsset* WorldChange) const
{
	return ActiveWorldChanges.Contains(WorldChange);
}

void UWorldStateSubsystem::SetWorldCheckpoint(int32 NewCheckpoint, bool bTriggerIntermediateCheckpoints)
{
	// Decrement to fit bounds of array.
	NewCheckpoint--;
	int32 WorldCheckpointCorrected = WorldCheckpoint - 1;
	
	if (!ensureAlwaysMsgf(WorldCheckpoints.IsValidIndex(NewCheckpoint),
		TEXT("Checkpoint %d is out of bounds. Min: 0, Max: %d"), NewCheckpoint, WorldCheckpoints.Num() - 1))
	{
		return;
	}

	// Same checkpoint. Ignore.
	if (NewCheckpoint == WorldCheckpointCorrected)
		return;

	if (NewCheckpoint < WorldCheckpointCorrected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Going back a World Checkpoint from %d to %d. This may cause inconsistencies"),
			WorldCheckpoint, NewCheckpoint + 1);
	}

	if (NewCheckpoint > WorldCheckpoint)
	{
		if (bTriggerIntermediateCheckpoints)
		{
			while (WorldCheckpoint <= NewCheckpoint)
				SetWorldCheckpoint(WorldCheckpoint + 1);
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Skipping a World Checkpoint from %d to %d. This may cause inconsistencies"),
			WorldCheckpoint, NewCheckpoint + 1)
	}
	
	const auto Checkpoint = WorldCheckpoints[NewCheckpoint];

	UKismetSystemLibrary::PrintString(this, FString::Printf(
		TEXT("Loading World Checkpoint: %d from %d"), NewCheckpoint + 1, WorldCheckpoint));

	UnloadDataLayers(Checkpoint->UnloadLayers);
	LoadDataLayers(Checkpoint->LoadLayers);

	WorldCheckpoint = ++NewCheckpoint;
}

void UWorldStateSubsystem::IncrementWorldCheckpoint()
{
	SetWorldCheckpoint(GetWorldCheckpoint() + 1);
}

void UWorldStateSubsystem::AddWorldChange(UWorldChangeDataAsset* WorldChange)
{
	if (!IsValid(WorldChange))
		return;
	
	if (!ensureAlwaysMsgf(!ActiveWorldChanges.Contains(WorldChange), TEXT("World Change already active.")))
		return;

	ActiveWorldChanges.Add(WorldChange);
	UnloadDataLayers(WorldChange->UnloadLayers);
	LoadDataLayers(WorldChange->LoadLayers);
}

void UWorldStateSubsystem::RemoveWorldChange(UWorldChangeDataAsset* WorldChange)
{
	if (!IsValid(WorldChange))
		return;

	int32 Index;
	if (!ActiveWorldChanges.Find(WorldChange, OUT Index))
		return;

	// Check which loaded layers are not currently loaded by another world change or checkpoint.
	TArray<UDataLayerAsset*> SoleDataLayers;

	// For every loaded layer of this world change.
	for (auto DataLayer : ActiveWorldChanges[Index]->LoadLayers)
	{
		// Check the other active world changes.
		bool bFound = false;
		for (UWorldChangeDataAsset* WorldChangeItr : ActiveWorldChanges)
		{
			// Self = skip.
			if (WorldChangeItr == ActiveWorldChanges[Index])
				continue;
			
			if (WorldChangeItr->LoadLayers.Contains(DataLayer))
			{
				bFound = true;
				break;
			}
		}

		// World Change was found loading this layer = ignore.
		if (bFound)
			continue;

		// Check world checkpoints from top to bottom.
		auto WorldCheckpointLoadedDataLayers = GetWorldCheckpointDataLayersToLoad_Uncached(WorldCheckpoint);
		if (WorldCheckpointLoadedDataLayers.Contains(DataLayer))
			continue;

		// None was found = safe to remove this layer.
		SoleDataLayers.Add(DataLayer);
	}

	UnloadDataLayers(SoleDataLayers);
}

void UWorldStateSubsystem::ClearWorldChanges()
{
	for (int32 i = ActiveWorldChanges.Num() - 1; i >= 0; i--)
		RemoveWorldChange(ActiveWorldChanges[i]);
}

TSet<UDataLayerAsset*> UWorldStateSubsystem::GetWorldCheckpointDataLayersToLoad_Uncached(int32 Checkpoint)
{
	Checkpoint--;

	if (!WorldCheckpoints.IsValidIndex(Checkpoint))
		return TSet<UDataLayerAsset*>();

	TSet<UDataLayerAsset*> Set;
	Set.Reserve(10);

	// For every Checkpoint, unload levels then load.
	for (int32 i = 0; i < Checkpoint + 1; i++)
	{
		const auto CheckpointStruct = WorldCheckpoints[Checkpoint];
		
		for (const auto DataLayerAsset : CheckpointStruct->UnloadLayers)
			if (Set.Contains(DataLayerAsset))
				Set.Remove(DataLayerAsset);
		
		for (const auto DataLayerAsset : CheckpointStruct->LoadLayers)
			Set.Add(DataLayerAsset);
	}

	return Set;
}

void UWorldStateSubsystem::UnloadDataLayers(const TArray<UDataLayerAsset*>& DataLayers) const
{
	const auto DLS = GetWorld()->GetSubsystem<UDataLayerSubsystem>();
	for (const auto Layer : DataLayers)
		DLS->SetDataLayerInstanceRuntimeState(Layer, EDataLayerRuntimeState::Unloaded);
}

void UWorldStateSubsystem::LoadDataLayers(const TArray<UDataLayerAsset*>& DataLayers) const
{
	const auto DLS = GetWorld()->GetSubsystem<UDataLayerSubsystem>();
	for (const auto Layer : DataLayers)
		DLS->SetDataLayerInstanceRuntimeState(Layer, EDataLayerRuntimeState::Activated);
}
