// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToPathPoint.generated.h"

/**
 * Behavior Tree Task to move the AI to the current path point specified in the Blackboard.
 * This task initiates movement and then returns Succeeded.
 * Arrival checking and path progression should be handled by a service (e.g., UBTService_UpdatePathProgress).
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UBTTask_MoveToPathPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_MoveToPathPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	// Blackboard key selectors
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PathTargetLocationKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PathCurrentIndexKey; // Used for logging/validation

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PathCompletedKey; // Used for validation

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float AcceptanceRadius;
};
