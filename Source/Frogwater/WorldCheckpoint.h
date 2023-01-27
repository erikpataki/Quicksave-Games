#pragma once
#include "Engine/DataTable.h"
#include "WorldCheckpoint.generated.h"

USTRUCT(BlueprintType)
struct FROGWATER_API FWorldCheckpoint : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	TArray<UDataLayerAsset*> LoadLayers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="World State")
	TArray<UDataLayerAsset*> UnloadLayers;

	/// Other stuff like darkness or whatever.
};
