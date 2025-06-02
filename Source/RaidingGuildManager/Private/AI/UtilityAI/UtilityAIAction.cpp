// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/UtilityAIAction.h"
#include "AI/UtilityAI/UtilityAIConsideration.h" // Though not directly used in this cpp, it's good for context and if Execute is expanded
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

void UUtilityAIAction::Execute(AController* Controller, APawn* Pawn)
{
	if (!Controller || !Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUtilityAIAction::Execute called with null Controller or Pawn for action %s."), *ActionName);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Executing action: %s on Pawn: %s controlled by Controller: %s"),
		*ActionName,
		Pawn ? *Pawn->GetName() : TEXT("None"),
		Controller ? *Controller->GetName() : TEXT("None"));

	// Further implementation would go here, specific to what the action does.
	// For example, telling the Pawn to move to a location, attack a target, etc.
}
