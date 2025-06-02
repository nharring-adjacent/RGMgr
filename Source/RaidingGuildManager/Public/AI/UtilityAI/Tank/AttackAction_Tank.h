// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIAction.h"
#include "AttackAction_Tank.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UAttackAction_Tank : public UUtilityAIAction
{
	GENERATED_BODY()

public:
	UAttackAction_Tank();

	virtual void Execute(AController* Controller, APawn* Pawn) override;
};
