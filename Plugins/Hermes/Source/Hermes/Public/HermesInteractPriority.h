// © 2021 Liam Hall

#pragma once

/**
* 
*/
UENUM(BlueprintType)
enum class EHermesInteractPriority : uint8
{
	Low 				UMETA(Description = "For interactions like picking up weapons."),
	Normal  			UMETA(Description = "For interactions like opening doors."),
	High  			    UMETA(Description = "For interactions like reviving players.")
};