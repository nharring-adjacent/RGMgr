// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIConsideration.h"
#include "TargetInRangeConsideration_Tank.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UTargetInRangeConsideration_Tank : public UUtilityAIConsideration
{
	GENERATED_BODY()

public:
	UTargetInRangeConsideration_Tank();

	virtual float ScoreConsideration(AController* Controller, APawn* Pawn) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange;
};
