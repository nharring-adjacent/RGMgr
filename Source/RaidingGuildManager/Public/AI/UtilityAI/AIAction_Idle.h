// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIAction.h"
#include "AIAction_Idle.generated.h"

/**
 * Utility AI Action: Makes the character idle.
 */
UCLASS(Blueprintable, BlueprintType)
class RAIDINGGUILDMANAGER_API UAIAction_Idle : public UUtilityAIAction
{
	GENERATED_BODY()

public:
	UAIAction_Idle();

	virtual void Execute(AController* Controller, APawn* Pawn) override;
};
