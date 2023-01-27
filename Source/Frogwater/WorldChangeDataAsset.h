// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldCheckpoint.h"
#include "Engine/DataAsset.h"
#include "WorldChangeDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API UWorldChangeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	TArray<UDataLayerAsset*> LoadLayers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	TArray<UDataLayerAsset*> UnloadLayers;
	
};
