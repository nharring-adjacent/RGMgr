// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BTTask_MoveToPathPoint.h"
#include "AI/RaiderAIController.h" // For GetCurrentPathPoints and type casting
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h" // For UNavigationSystemV1
#include "GameFramework/Pawn.h" // To get Pawn location for logging (optional)

UBTTask_MoveToPathPoint::UBTTask_MoveToPathPoint()
{
	NodeName = "Move To Path Point";
	AcceptanceRadius = 50.0f; // Default acceptance radius

	// Initialize Blackboard Key Selectors
	PathTargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToPathPoint, PathTargetLocationKey));
	PathCurrentIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToPathPoint, PathCurrentIndexKey));
	PathCompletedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToPathPoint, PathCompletedKey));
}

EBTNodeResult::Type UBTTask_MoveToPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARaiderAIController* AIController = Cast<ARaiderAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !MyBlackboard)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_MoveToPathPoint: AIController or BlackboardComponent is null."));
		return EBTNodeResult::Failed;
	}

	if (MyBlackboard->GetValueAsBool(PathCompletedKey.SelectedKeyName))
	{
		UE_LOG(LogTemp, Log, TEXT("UBTTask_MoveToPathPoint: Path is already marked as completed. Task Succeeded (no move)."));
		return EBTNodeResult::Succeeded; // Path is done, no more points to move to.
	}

	int32 CurrentIndex = MyBlackboard->GetValueAsInt(PathCurrentIndexKey.SelectedKeyName);
	const TArray<FVector2D>& PathPoints = AIController->GetCurrentPathPoints();

	if (!PathPoints.IsValidIndex(CurrentIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_MoveToPathPoint: CurrentIndex %d is invalid for PathPoints of size %d. Marking path completed."), CurrentIndex, PathPoints.Num());
		MyBlackboard->SetValueAsBool(PathCompletedKey.SelectedKeyName, true);
		return EBTNodeResult::Succeeded; // Path exhausted or index issue
	}

	// Get the target location from the Blackboard, which should have been set by UBTService_UpdatePathProgress or initial setup
	FVector TargetLocation = MyBlackboard->GetValueAsVector(PathTargetLocationKey.SelectedKeyName);

	// Validate if TargetLocation is valid (not FVector::ZeroVector unless that's a valid path point)
	// For simplicity, we assume any vector from BB is the intended target.
	UE_LOG(LogTemp, Log, TEXT("BTTask_MoveToPathPoint %s: Moving to %s (Index: %d)."), *AIController->GetNameSafe(AIController->GetPawn()), *TargetLocation.ToString(), CurrentIndex);

	EPathFollowingRequestResult::Type MoveRequestResult = AIController->MoveToLocation(TargetLocation, AcceptanceRadius, true, true, false, true, nullptr, true);

	switch(MoveRequestResult)
	{
		case EPathFollowingRequestResult::RequestSuccessful:
			UE_LOG(LogTemp, Log, TEXT("BTTask_MoveToPathPoint %s: MoveToLocation requested for index %d to %s. Task Succeeded (movement initiated)."), *AIController->GetNameSafe(AIController->GetPawn()), CurrentIndex, *TargetLocation.ToString());
			return EBTNodeResult::Succeeded;
		case EPathFollowingRequestResult::AlreadyAtGoal:
			UE_LOG(LogTemp, Log, TEXT("BTTask_MoveToPathPoint %s: Already at goal for index %d at %s. Task Succeeded."), *AIController->GetNameSafe(AIController->GetPawn()), CurrentIndex, *TargetLocation.ToString());
			return EBTNodeResult::Succeeded;
		case EPathFollowingRequestResult::Failed:
		default:
			UE_LOG(LogTemp, Warning, TEXT("BTTask_MoveToPathPoint %s: MoveToLocation failed for index %d to %s. Task Failed."), *AIController->GetNameSafe(AIController->GetPawn()), CurrentIndex, *TargetLocation.ToString());
			return EBTNodeResult::Failed;
	}
}

FString UBTTask_MoveToPathPoint::GetStaticDescription() const
{
	return FString::Printf(TEXT("Move To Path Point: TargetKey='%s', IdxKey='%s', DoneKey='%s', Radius=%.1f"),
		*PathTargetLocationKey.SelectedKeyName.ToString(),
		*PathCurrentIndexKey.SelectedKeyName.ToString(),
		*PathCompletedKey.SelectedKeyName.ToString(),
		AcceptanceRadius
	);
}
