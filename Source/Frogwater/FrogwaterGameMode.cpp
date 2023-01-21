// Copyright Epic Games, Inc. All Rights Reserved.

#include "FrogwaterGameMode.h"
#include "FrogwaterCharacter.h"
#include "FrogwaterGameState.h"
#include "FrogwaterHUD.h"
#include "FrogwaterPlayerController.h"
#include "MainCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFrogwaterGameMode::AFrogwaterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	DefaultPawnClass = AMainCharacter::StaticClass();
	PlayerControllerClass = AFrogwaterPlayerController::StaticClass();
	HUDClass = AFrogwaterHUD::StaticClass();
	GameStateClass = AFrogwaterGameState::StaticClass();

}
