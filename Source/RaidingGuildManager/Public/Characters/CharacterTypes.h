#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
    None		UMETA(DisplayName = "None"),
    MeleeDPS    UMETA(DisplayName = "Melee DPS"),
    RangedDPS   UMETA(DisplayName = "Ranged DPS"),
    Tank        UMETA(DisplayName = "Tank"),
    Healer      UMETA(DisplayName = "Healer")
};

UENUM(BlueprintType)
enum class ETemporaryDirective : uint8
{
	None		UMETA(DisplayName = "None"),
	Taunt		UMETA(DisplayName = "Taunt"),
	Attack		UMETA(DisplayName = "Attack")
	// Add more directives as needed, e.g., MoveTo, DefendTarget
};
