// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIConsideration.h"
// Required for ARGMCharacterBase and URGMAttributeSet, will be included in CPP, but good to note dependencies.
#include "Consideration_HealthPercentage.generated.h"

/**
 * Utility AI Consideration: Scores based on the Pawn's health percentage.
 */
UCLASS(Blueprintable, BlueprintType)
class RAIDINGGUILDMANAGER_API UConsideration_HealthPercentage : public UUtilityAIConsideration
{
	GENERATED_BODY()

public:
	virtual float ScoreConsideration(AController* Controller, APawn* Pawn) override;
};
