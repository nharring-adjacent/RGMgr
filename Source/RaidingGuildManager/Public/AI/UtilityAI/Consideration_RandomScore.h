// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIConsideration.h"
#include "Consideration_RandomScore.generated.h"

/**
 * Utility AI Consideration: Returns a random score.
 */
UCLASS(Blueprintable, BlueprintType)
class RAIDINGGUILDMANAGER_API UConsideration_RandomScore : public UUtilityAIConsideration
{
	GENERATED_BODY()

public:
	virtual float ScoreConsideration(AController* Controller, APawn* Pawn) override;
};
