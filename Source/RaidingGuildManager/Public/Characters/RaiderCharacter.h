// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Characters/CharacterTypes.h"
#include "Characters/PersonalityStats.h"
#include "Characters/RGMCharacterBase.h"
#include "CoreMinimal.h"
#include "RaiderCharacter.generated.h"

class ARaiderAIController; // Forward declaration

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ARaiderCharacter : public ARGMCharacterBase {
  GENERATED_BODY()

public:
  ARaiderCharacter();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Class")
  ECharacterClass CharacterClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
  FPersonalityStats PersonalityStats;

protected:
  // Overrides from RGMCharacterBase
  // virtual void InitializeAttributes() override;
  // virtual void GiveDefaultAbilities() override;
};
