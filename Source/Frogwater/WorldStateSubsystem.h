// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldCheckpoint.h"
#include "Engine/DataTable.h"
#include "WorldStateSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API UWorldStateSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

public:
	UFUNCTION(BlueprintPure, Category="World State")
	int32 GetWorldCheckpoint() const { return WorldCheckpoint; }
	
	UFUNCTION(BlueprintPure, Category="World State")
	bool IsWorldChangeActive(UWorldChangeDataAsset* WorldChange) const;

	
	UFUNCTION(BlueprintCallable, Category="World State")
	void SetWorldCheckpoint(int32 NewCheckpoint, bool bTriggerIntermediateCheckpoints = false);

	UFUNCTION(BlueprintCallable, Category="World State")
	void IncrementWorldCheckpoint();

	UFUNCTION(BlueprintCallable, Category="World State")
	void AddWorldChange(UWorldChangeDataAsset* WorldChange);

	// Doesn't revert any Unloads.
	UFUNCTION(BlueprintCallable, Category="World State")
	void RemoveWorldChange(UWorldChangeDataAsset* WorldChange);

	UFUNCTION(BlueprintCallable, Category="World State")
	void ClearWorldChanges();

	UFUNCTION(BlueprintPure, Category="World State")
	TSet<UDataLayerAsset*> GetWorldCheckpointDataLayersToLoad_Uncached(int32 Checkpoint);

protected:
	void UnloadDataLayers(const TArray<UDataLayerAsset*>& DataLayers) const;
	void LoadDataLayers(const TArray<UDataLayerAsset*>& DataLayers) const;

protected:
	UPROPERTY(VisibleInstanceOnly, Category="World State")
	int32 WorldCheckpoint;
	
	UPROPERTY(VisibleInstanceOnly, Category="World State")
	TArray<UWorldChangeDataAsset*> ActiveWorldChanges;

	TArray<const FWorldCheckpoint*> WorldCheckpoints;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWorldCheckpointChangedSignature,
		const int32, CheckpointIndex,
		const FWorldCheckpoint&, CheckpointData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldChangeSignature,
		const UWorldChangeDataAsset*, WorldChange);

	UPROPERTY(BlueprintAssignable, Category="World State")
	FWorldCheckpointChangedSignature OnWorldCheckpointChanged;

	UPROPERTY(BlueprintAssignable, Category="World State")
	FWorldChangeSignature OnWorldChangeAdded;

	UPROPERTY(BlueprintAssignable, Category="World State")
	FWorldChangeSignature OnWorldChangeRemoved;
};
