// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdatePathProgress.generated.h"

/**
 * Behavior Tree Service to update the AI's progress along a predefined path.
 * It checks if the AI has reached its current path point. If so, it updates
 * the Blackboard with the next point or marks the path as completed.
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UBTService_UpdatePathProgress : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_UpdatePathProgress();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PathTargetLocationKey; // BB Key for current FVector target

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PathCurrentIndexKey; // BB Key for current path index (int)

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PathCompletedKey; // BB Key for path completion status (bool)

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector HasPathKey; // BB Key to check if a path exists (bool)

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector CurrentActionTagKey; // BB Key for the action tag of the current path point

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector CurrentActionTargetKey; // BB Key for the target actor of the current path point

	UPROPERTY(EditAnywhere, Category = "Node")
	float AcceptanceRadius;
};
