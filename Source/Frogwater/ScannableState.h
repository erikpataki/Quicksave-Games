#pragma once

UENUM(BlueprintType)
enum class EScannableState : uint8
{
	Unfocused,
	Focusing,
	Focused,
	Selected
};
