// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/AIAction_Idle.h"
#include "AI/UtilityAI/Consideration_HealthPercentage.h" // Will be needed for Step 5
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

UAIAction_Idle::UAIAction_Idle()
{
	ActionName = TEXT("IdleAction");
	// Step 5: Add UConsideration_HealthPercentage::StaticClass() to Considerations
	Considerations.Add(UConsideration_HealthPercentage::StaticClass());
}

void UAIAction_Idle::Execute(AController* Controller, APawn* Pawn)
{
	Super::Execute(Controller, Pawn); // Call parent implementation

	if (Pawn)
	{
		UE_LOG(LogTemp, Log, TEXT("Executing Idle Action for Pawn: %s"), *Pawn->GetName());
		// In a real scenario, this might involve stopping movement, playing an idle animation, etc.
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Executing Idle Action (Pawn is null)"));
	}
}
