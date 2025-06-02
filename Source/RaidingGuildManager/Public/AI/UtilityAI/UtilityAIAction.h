// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UtilityAIAction.generated.h"

/**
 * Base class for all Utility AI Actions.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class RAIDINGGUILDMANAGER_API UUtilityAIAction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
	FString ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility AI")
	TArray<TSubclassOf<class UUtilityAIConsideration>> Considerations;

	virtual void Execute(AController* Controller, APawn* Pawn);
};
