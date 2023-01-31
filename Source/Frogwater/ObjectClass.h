#pragma once
#include "Engine/DataTable.h"
#include "WorldCheckpoint.generated.h"

USTRUCT(BlueprintType)
struct FROGWATER_API FObjectClass : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UObject> Class;
};
