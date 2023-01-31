// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimaryAssetActor.h"

// Sets default values
APrimaryAssetActor::APrimaryAssetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto PrimaryAssetId = AActor::GetPrimaryAssetId();
	AssetType = "Item";
	AssetName = PrimaryAssetId.PrimaryAssetName;
}

FPrimaryAssetId APrimaryAssetActor::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, AssetName);
}
