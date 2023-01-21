// © 2021 Liam Hall

#pragma once

/**
* 
*/
UENUM(BlueprintType)
enum class EHermesInteractMode : uint8
{
	None 				UMETA(Description = "Not interactable."),
	Auto  			    UMETA(Description = "Automatically interacted with when collided."),
	Manual  			UMETA(Description = "Requires explicit interaction to be interacted with.")
};