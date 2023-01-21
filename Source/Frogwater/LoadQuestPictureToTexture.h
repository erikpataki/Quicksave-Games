// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LoadQuestPictureToTexture.generated.h"

class UQuestDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestPictureLoaded,
	const UTexture2D*, Picture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestPictureNotFound);

/**
 * 
 */
UCLASS()
class FROGWATER_API ULoadQuestPictureToTexture : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FQuestPictureLoaded PictureLoaded;

	UPROPERTY(BlueprintAssignable)
	FQuestPictureNotFound PictureNotFound;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Flow Control")
	static ULoadQuestPictureToTexture* LoadQuestPicture(const UObject* WorldContextObject, const UQuestDataAsset* Quest);

private:
	const UObject* WorldContext;
	const UQuestDataAsset* QuestToLoad = nullptr;

public:
	virtual void Activate() override;
};
