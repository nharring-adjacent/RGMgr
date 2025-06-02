// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/AIAction_Log.h"
#include "AI/UtilityAI/Consideration_RandomScore.h" // Will be needed for Step 5
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UAIAction_Log::UAIAction_Log()
{
	ActionName = TEXT("LogDebugMessageAction");
	// Step 5: Add UConsideration_RandomScore::StaticClass() to Considerations
	Considerations.Add(UConsideration_RandomScore::StaticClass());
}

void UAIAction_Log::Execute(AController* Controller, APawn* Pawn)
{
	Super::Execute(Controller, Pawn); // Call parent implementation

	if (Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executing LogDebugMessage Action for Pawn: %s"), *Pawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Executing LogDebugMessage Action (Pawn is null)"));
	}
}
