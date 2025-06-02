// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/UtilityAI/UtilityAIAction.h"
#include "AIAction_Log.generated.h"

/**
 * Utility AI Action: Logs a debug message.
 */
UCLASS(Blueprintable, BlueprintType)
class RAIDINGGUILDMANAGER_API UAIAction_Log : public UUtilityAIAction
{
	GENERATED_BODY()

public:
	UAIAction_Log();

	virtual void Execute(AController* Controller, APawn* Pawn) override;
};
