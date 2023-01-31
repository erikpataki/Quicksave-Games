// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PrimaryAssetActor.generated.h"

UCLASS()
class FROGWATER_API APrimaryAssetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APrimaryAssetActor();
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Asset Manager")
	FName AssetType;

	UPROPERTY(EditDefaultsOnly, Category="Asset Manager")
	FName AssetName;
};
