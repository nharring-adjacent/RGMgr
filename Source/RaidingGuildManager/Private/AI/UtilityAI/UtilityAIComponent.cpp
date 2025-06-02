// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/UtilityAIComponent.h"
#include "AI/UtilityAI/UtilityAIConsideration.h" // Required for UUtilityAIConsideration
#include "GameFramework/Controller.h" // Required for AController
#include "GameFramework/Pawn.h" // Required for APawn

// Sets default values for this component's properties
UUtilityAIComponent::UUtilityAIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false; // Typically, AI decision making doesn't need to tick every frame. It's event driven or ticked at a lower frequency.

	// ...
}


// Called when the game starts
void UUtilityAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UUtilityAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UUtilityAIAction* UUtilityAIComponent::SelectBestAction(AController* Controller, APawn* Pawn)
{
	UUtilityAIAction* BestAction = nullptr;
	float HighestScore = 0.0f;

	if (!Controller || !Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectBestAction called with null Controller or Pawn."));
		return nullptr;
	}

	for (TSubclassOf<UUtilityAIAction> ActionClass : AvailableActions)
	{
		if (!ActionClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("Null ActionClass in AvailableActions."));
			continue;
		}

		UUtilityAIAction* ActionInstance = NewObject<UUtilityAIAction>(this, ActionClass);
		if (!ActionInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create ActionInstance of class %s."), *ActionClass->GetName());
			continue;
		}

		float CurrentActionScore = 0.0f;
		int32 ConsiderationCount = ActionInstance->Considerations.Num();

		if (ConsiderationCount == 0)
		{
			CurrentActionScore = 0.5f; // Default score for actions with no considerations
			UE_LOG(LogTemp, Log, TEXT("Action %s has no considerations, assigned default score %f"), *ActionInstance->ActionName, CurrentActionScore);
		}
		else
		{
			float TotalScore = 0.0f;
			for (TSubclassOf<UUtilityAIConsideration> ConsiderationClass : ActionInstance->Considerations)
			{
				if (!ConsiderationClass)
				{
					UE_LOG(LogTemp, Warning, TEXT("Null ConsiderationClass in Action %s."), *ActionInstance->ActionName);
					continue;
				}
				UUtilityAIConsideration* ConsiderationInstance = NewObject<UUtilityAIConsideration>(ActionInstance, ConsiderationClass); // Outer is ActionInstance
				if (!ConsiderationInstance)
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to create ConsiderationInstance of class %s for Action %s."), *ConsiderationClass->GetName(), *ActionInstance->ActionName);
					continue;
				}
				TotalScore += ConsiderationInstance->ScoreConsideration(Controller, Pawn);
			}
			CurrentActionScore = TotalScore / ConsiderationCount;
			UE_LOG(LogTemp, Log, TEXT("Action %s scored %f based on %d considerations."), *ActionInstance->ActionName, CurrentActionScore, ConsiderationCount);
		}

		if (CurrentActionScore > HighestScore)
		{
			HighestScore = CurrentActionScore;
			BestAction = ActionInstance;
		}
	}

	if (BestAction)
	{
		UE_LOG(LogTemp, Log, TEXT("Selected best action: %s with score %f"), *BestAction->ActionName, HighestScore);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No suitable action found or all actions scored 0."));
	}

	// Return the instance of the best action. If no action scores above 0, this will be nullptr.
	// The caller is responsible for managing the lifecycle of this returned object if it's not null.
	// However, since actions are UObjects created with an outer (this component), they will be garbage collected when the component is.
	// If an action is selected and executed, it might destroy itself or be kept for a duration.
	// For this initial framework, we'll assume the caller will execute it and then it can be GC'd.
	return BestAction;
}
