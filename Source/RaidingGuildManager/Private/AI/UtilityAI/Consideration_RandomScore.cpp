// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Consideration_RandomScore.h"
#include "Math/UnrealMathUtility.h" // For FMath::FRand()
#include "GameFramework/Controller.h" // For AController (though not directly used in this simple version, good practice)
#include "GameFramework/Pawn.h" // For APawn (though not directly used, good practice)

float UConsideration_RandomScore::ScoreConsideration(AController* Controller, APawn* Pawn)
{
	// Pawn and Controller are not used in this simple random score, but they are part of the signature.
	// You could add logging here if needed:
	// UE_LOG(LogTemp, Verbose, TEXT("UConsideration_RandomScore scoring for Pawn: %s"), Pawn ? *Pawn->GetName() : TEXT("None"));

	return FMath::FRand();
}
