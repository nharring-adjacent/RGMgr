// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h" // For FGameplayTag
#include "CommandParser.generated.h"

// Forward declarations
class ARGMCharacterBase;
class IGameplayAbilityActorInterface; // For TScriptInterface

/**
 * UCommandParser
 *
 * Responsible for taking raw text (presumably from voice input) and attempting
 * to parse it into a structured game command, including a command verb (tag),
 * an optional target actor, and an optional target location.
 */
UCLASS(Blueprintable) // Blueprintable to allow for Blueprint-based parser extensions if desired
class RAIDINGGUILDMANAGER_API UCommandParser : public UObject
{
	GENERATED_BODY()

public:
	UCommandParser();

	/**
	 * Parses raw text into a gameplay command.
	 * @param WorldContextObject The world context (e.g., an Actor or the World itself).
	 * @param RawText The raw text input to parse.
	 * @param OutCommandTag The primary verb or action of the command (e.g., "Command.Verb.Attack").
	 * @param OutTargetActor The identified target actor for the command, if any.
	 * @param OutTargetLocation The identified target location for the command, if any.
	 * @return True if a command was successfully parsed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice Commands", meta = (WorldContext = "WorldContextObject"))
	virtual bool ParseCommand(UObject* WorldContextObject, const FString& RawText,
							  FGameplayTag& OutCommandTag,
							  TScriptInterface<IGameplayAbilityActorInterface>& OutTargetActor,
							  FVector& OutTargetLocation);
};
