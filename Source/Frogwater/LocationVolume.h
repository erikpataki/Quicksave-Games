// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "LocationVolume.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FROGWATER_API ALocationVolume : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=Location)
	FText LocationName;
	
};
