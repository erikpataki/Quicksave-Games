// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PrimaryAssetPawn.generated.h"

UCLASS()
class FROGWATER_API APrimaryAssetPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APrimaryAssetPawn();
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Asset Manager")
	FName AssetType;

	UPROPERTY(EditDefaultsOnly, Category="Asset Manager")
	FName AssetName;
};
