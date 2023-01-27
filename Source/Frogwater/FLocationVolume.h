// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "FLocationVolume.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FROGWATER_API AFLocationVolume : public ATriggerVolume
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category=Location)
	FText LocationName;
	
};
