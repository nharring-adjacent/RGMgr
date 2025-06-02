// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIAction.h"
#include "TauntAction_Tank.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UTauntAction_Tank : public UUtilityAIAction
{
	GENERATED_BODY()

public:
	UTauntAction_Tank();

	virtual void Execute(AController* Controller, APawn* Pawn) override;
};
