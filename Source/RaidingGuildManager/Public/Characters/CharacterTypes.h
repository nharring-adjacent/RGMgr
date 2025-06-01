#pragma once

#include "CharacterTypes.generated.h"
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8 {
  None UMETA(DisplayName = "None"),
  MeleeDPS UMETA(DisplayName = "Melee DPS"),
  RangedDPS UMETA(DisplayName = "Ranged DPS"),
  Tank UMETA(DisplayName = "Tank"),
  Healer UMETA(DisplayName = "Healer")
};
