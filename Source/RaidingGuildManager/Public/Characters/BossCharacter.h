// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RGMCharacterBase.h"
#include "BossCharacter.generated.h"

class ABossAIController; // Forward declaration

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ABossCharacter : public ARGMCharacterBase
{
	GENERATED_BODY()

public:
	ABossCharacter();

protected:
	// Overrides from RGMCharacterBase
	// virtual void InitializeAttributes() override;
	// virtual void GiveDefaultAbilities() override;

};
