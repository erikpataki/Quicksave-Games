// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrogwaterStatics.generated.h"

/**
 * 
 */
UCLASS()
class FROGWATER_API UFrogwaterStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category=Maths)
	static FRotator AddRotator(const FRotator& A, const FRotator& B);

	UFUNCTION(BlueprintPure, Category=Maths)
	static bool IsPointInBox(const FVector& Point, const FBox& Box);
};
