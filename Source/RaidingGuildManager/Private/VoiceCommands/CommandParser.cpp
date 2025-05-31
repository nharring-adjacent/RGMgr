// Copyright Epic Games, Inc. All Rights Reserved.

#include "VoiceCommands/CommandParser.h"
#include "GameplayTagsManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/RGMCharacterBase.h"  // For casting to find specific character types
#include "Characters/BossCharacter.h"
#include "Characters/RaiderCharacter.h"
#include "AbilitySystemInterface.h" // For TScriptInterface

UCommandParser::UCommandParser()
{
}

bool UCommandParser::ParseCommand(UObject* WorldContextObject, const FString& RawText,
                                  FGameplayTag& OutCommandTag,
                                  TScriptInterface<IGameplayAbilityActorInterface>& OutTargetActor,
                                  FVector& OutTargetLocation)
{
	OutCommandTag = FGameplayTag::EmptyTag;
	OutTargetActor = nullptr;
	OutTargetLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("CommandParser: Attempting to parse raw text: '%s'"), *RawText);

	if (RawText.IsEmpty() || !WorldContextObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("CommandParser: RawText is empty or WorldContextObject is null."));
		return false;
	}

	FString ProcessedText = RawText.ToLower();
	bool bCommandParsed = false;

	// Example Keyword Matching for Command Verbs
	if (ProcessedText.Contains(TEXT("attack")) || ProcessedText.Contains(TEXT("fight")))
	{
		OutCommandTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Command.Verb.Attack"));
		bCommandParsed = true;

		// Target Parsing for "attack" command
		if (ProcessedText.Contains(TEXT("boss")))
		{
			ABossCharacter* Boss = Cast<ABossCharacter>(UGameplayStatics::GetActorOfClass(WorldContextObject, ABossCharacter::StaticClass()));
			if (Boss)
			{
				OutTargetActor = Boss;
				UE_LOG(LogTemp, Log, TEXT("CommandParser: Parsed 'attack boss'. Target: %s"), *Boss->GetName());
			}
		}
		// Add more specific target parsing here: by name, role, "nearest enemy", etc.
		// Example: Find any RGMCharacterBase if no specific target mentioned but it's an attack
		else if (OutTargetActor == nullptr)
		{
			// Simple fallback: target any available RGMCharacterBase (could be ally or enemy based on context)
			// This needs refinement based on who is issuing the command and typical targets.
			// For now, let's assume it targets an enemy if not specified.
			// If this parser is on a player controller, it might target the player's current target.
			// If on an AI, it might target its own current enemy.
			// This is a placeholder for more sophisticated targeting.
			TArray<AActor*> FoundCharacters;
			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ARGMCharacterBase::StaticClass(), FoundCharacters);
			for (AActor* Actor : FoundCharacters)
			{
				// This logic is very basic: just picks the first character that isn't the commander (if known)
				// and could be a boss or an opposing raider.
				// A real system would need faction checks or specific "enemy" queries.
				ARGMCharacterBase* PotentialTarget = Cast<ARGMCharacterBase>(Actor);
				if (PotentialTarget) // && PotentialTarget->IsHostileTowards(Commander) )
				{
					OutTargetActor = PotentialTarget;
					UE_LOG(LogTemp, Log, TEXT("CommandParser: Parsed 'attack', fallback target: %s"), *PotentialTarget->GetName());
					break;
				}
			}
		}
	}
	else if (ProcessedText.Contains(TEXT("move to")) || ProcessedText.Contains(TEXT("go to")))
	{
		OutCommandTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Command.Verb.MoveTo"));
		bCommandParsed = true;
		// Crude location parsing example (highly simplified)
		// "move to x 100 y 200 z 50"
		FRegexPattern Pattern(TEXT("x\\s*([+-]?\\d+\\.?\\d*)\\s*y\\s*([+-]?\\d+\\.?\\d*)\\s*z\\s*([+-]?\\d+\\.?\\d*)"));
		FRegexMatcher Matcher(Pattern, ProcessedText);
		if (Matcher.FindNext())
		{
			OutTargetLocation.X = FCString::Atof(*Matcher.GetCaptureGroup(1));
			OutTargetLocation.Y = FCString::Atof(*Matcher.GetCaptureGroup(2));
			OutTargetLocation.Z = FCString::Atof(*Matcher.GetCaptureGroup(3));
			UE_LOG(LogTemp, Log, TEXT("CommandParser: Parsed 'move to' location: %s"), *OutTargetLocation.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CommandParser: 'move to' command detected, but location parsing failed or not implemented for: \"%s\""), *ProcessedText);
			// Fallback: move to a default location or player's current target's location if available
		}
	}
	else if (ProcessedText.Contains(TEXT("select target")) || (ProcessedText.Contains(TEXT("target")) && !bCommandParsed) ) // "target" alone if not already part of "attack target"
	{
		OutCommandTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Command.Verb.SelectTarget"));
		bCommandParsed = true;
		// Target parsing logic similar to "attack"
		if (ProcessedText.Contains(TEXT("boss")))
		{
			ABossCharacter* Boss = Cast<ABossCharacter>(UGameplayStatics::GetActorOfClass(WorldContextObject, ABossCharacter::StaticClass()));
			if (Boss)
			{
				OutTargetActor = Boss;
				UE_LOG(LogTemp, Log, TEXT("CommandParser: Parsed 'select target boss'. Target: %s"), *Boss->GetName());
			}
		}
		// Add more specific target parsing...
	}
	// Add more command verbs like "heal", "defend", "follow", "stop" etc.


	// Log final parsed result before returning
	if (bCommandParsed && OutCommandTag.IsValid())
	{
		AActor* TargetAsActor = Cast<AActor>(OutTargetActor.GetObject()); // Try to cast to AActor for GetNameSafe
		UE_LOG(LogTemp, Log, TEXT("CommandParser: Parsed result - Tag: %s, Target: %s, Location: %s"),
			*OutCommandTag.ToString(),
			*GetNameSafe(TargetAsActor), // Use GetNameSafe on the AActor pointer
			*OutTargetLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CommandParser: Failed to parse a valid command from text: \"%s\". Final Tag: %s"), *RawText, *OutCommandTag.ToString());
	}

	return bCommandParsed && OutCommandTag.IsValid();
}
