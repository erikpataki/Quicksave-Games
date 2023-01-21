// © 2021 Liam Hall

#pragma once

/**
* 
*/
UENUM(BlueprintType)
enum class EHermesNetMode : uint8
{
	None 				UMETA(Description = "None."),
	Client  			UMETA(Description = "Affects only remote clients."),
	Server  			UMETA(Description = "Affects only the server."),
	Multicast           UMETA(Description = "Affects everyone.")
};