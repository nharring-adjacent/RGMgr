// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UtilityAIConsideration.generated.h"

/**
 * Base class for all Utility AI Considerations.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class RAIDINGGUILDMANAGER_API UUtilityAIConsideration : public UObject
{
	GENERATED_BODY()

public:
	virtual float ScoreConsideration(AController* Controller, APawn* Pawn) PURE_VIRTUAL(UUtilityAIConsideration::ScoreConsideration, return 0.0f;);
};
