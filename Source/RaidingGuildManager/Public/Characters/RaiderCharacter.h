// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RGMCharacterBase.h"
#include "RaiderCharacter.generated.h"

class ARaiderAIController; // Forward declaration

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ARaiderCharacter : public ARGMCharacterBase
{
	GENERATED_BODY()

public:
	ARaiderCharacter();

protected:
	// Overrides from RGMCharacterBase
	// virtual void InitializeAttributes() override;
	// virtual void GiveDefaultAbilities() override;

};
