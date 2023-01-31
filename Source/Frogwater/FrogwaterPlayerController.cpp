// Fill out your copyright notice in the Description page of Project Settings.


#include "FrogwaterPlayerController.h"

#include "FWCheatManager.h"

AFrogwaterPlayerController::AFrogwaterPlayerController(const FObjectInitializer& ObjectInitializer)
{
	CheatClass = UFWCheatManager::StaticClass();
}
