// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIConsideration.h"
#include "LowThreatConsideration_Tank.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ULowThreatConsideration_Tank : public UUtilityAIConsideration
{
	GENERATED_BODY()

public:
	ULowThreatConsideration_Tank();

	virtual float ScoreConsideration(AController* Controller, APawn* Pawn) override;

protected:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	// float ThreatThresholdFactor; // e.g., Tank's threat should be < HighestThreat * ThreatThresholdFactor
};
