// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FrogwaterGameMode.generated.h"

UCLASS(minimalapi)
class AFrogwaterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFrogwaterGameMode();
	
protected:
	virtual void BeginPlay() override;
};



