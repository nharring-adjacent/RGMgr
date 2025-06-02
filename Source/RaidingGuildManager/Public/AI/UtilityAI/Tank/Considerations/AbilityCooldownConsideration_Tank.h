// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIConsideration.h"
#include "AbilityCooldownConsideration_Tank.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UAbilityCooldownConsideration_Tank : public UUtilityAIConsideration
{
	GENERATED_BODY()

public:
	UAbilityCooldownConsideration_Tank();

	virtual float ScoreConsideration(AController* Controller, APawn* Pawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName AbilityName;
};
