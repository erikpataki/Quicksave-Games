// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimaryAssetPawn.h"


// Sets default values
APrimaryAssetPawn::APrimaryAssetPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto PrimaryAssetId = APawn::GetPrimaryAssetId();
	AssetType = "Item";
	AssetName = PrimaryAssetId.PrimaryAssetName;
}

FPrimaryAssetId APrimaryAssetPawn::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, AssetName);
}