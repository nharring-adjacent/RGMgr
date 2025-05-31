// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BTService_UpdatePathProgress.h"
#include "AI/RaiderAIController.h" // For GetCurrentPathPoints and type casting
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"       // For getting Pawn location
#include "Math/Vector.h"            // For FVector::DistSquared

UBTService_UpdatePathProgress::UBTService_UpdatePathProgress()
{
	NodeName = "Update Path Progress";
	Interval = 0.25f; // How often to tick
	RandomDeviation = 0.05f;
	AcceptanceRadius = 50.0f; // Default acceptance radius, should match MoveTo task

#include "Planning/RaidPlanDataTypes.h" // For FRaidPlanPointData

UBTService_UpdatePathProgress::UBTService_UpdatePathProgress()
{
	NodeName = "Update Path Progress";
	Interval = 0.25f;
	RandomDeviation = 0.05f;
	AcceptanceRadius = 50.0f;

	// Initialize Blackboard Key Selectors
	PathTargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdatePathProgress, PathTargetLocationKey));
	PathCurrentIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdatePathProgress, PathCurrentIndexKey));
	PathCompletedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdatePathProgress, PathCompletedKey));
	HasPathKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdatePathProgress, HasPathKey));
	CurrentActionTagKey.AddTagFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdatePathProgress, CurrentActionTagKey));
	CurrentActionTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdatePathProgress, CurrentActionTargetKey), AActor::StaticClass());
}

void UBTService_UpdatePathProgress::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARaiderAIController* AIController = Cast<ARaiderAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !MyBlackboard)
	{
		return;
	}

	if (!MyBlackboard->GetValueAsBool(HasPathKey.SelectedKeyName))
	{
		if(!MyBlackboard->GetValueAsBool(PathCompletedKey.SelectedKeyName))
		{
			MyBlackboard->SetValueAsBool(PathCompletedKey.SelectedKeyName, true);
		}
		return;
	}

	if (MyBlackboard->GetValueAsBool(PathCompletedKey.SelectedKeyName))
	{
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	int32 CurrentIndex = MyBlackboard->GetValueAsInt(PathCurrentIndexKey.SelectedKeyName);
	const TArray<FRaidPlanPointData>& PathPoints = AIController->GetCurrentPathPoints(); // Now returns TArray<FRaidPlanPointData>

	if (!PathPoints.IsValidIndex(CurrentIndex))
	{
		MyBlackboard->SetValueAsBool(PathCompletedKey.SelectedKeyName, true);
		return;
	}

	FVector CurrentPawnLocation = ControlledPawn->GetActorLocation();
	// Current target location on path (only X and Y are from path, Z is pawn's current Z for 2D distance check)
	FVector TargetPathPointLocation = FVector(PathPoints[CurrentIndex].Location.X, PathPoints[CurrentIndex].Location.Y, CurrentPawnLocation.Z);

	if (FVector::DistSquared(CurrentPawnLocation, TargetPathPointLocation) < FMath::Square(AcceptanceRadius))
	{
		CurrentIndex++;
		if (PathPoints.IsValidIndex(CurrentIndex))
		{
			const FRaidPlanPointData& NextPointData = PathPoints[CurrentIndex];
			MyBlackboard->SetValueAsInt(PathCurrentIndexKey.SelectedKeyName, CurrentIndex);
			MyBlackboard->SetValueAsVector(PathTargetLocationKey.SelectedKeyName, FVector(NextPointData.Location.X, NextPointData.Location.Y, CurrentPawnLocation.Z));
			MyBlackboard->SetValueAsTag(CurrentActionTagKey.SelectedKeyName, NextPointData.ActionTag);
			AIController->ResolveAndSetTarget(NextPointData.TargetIdentifier, CurrentActionTargetKey.SelectedKeyName);

			UE_LOG(LogTemp, Log, TEXT("BTService_UpdatePathProgress %s: Reached point %d. New TargetLocation: %s, Action: %s, TargetID for Resolve: %s"),
				*AIController->GetNameSafe(AIController->GetPawn()), CurrentIndex, *(MyBlackboard->GetValueAsVector(PathTargetLocationKey.SelectedKeyName).ToString()), *NextPointData.ActionTag.ToString(), *NextPointData.TargetIdentifier);
		}
		else
		{
			MyBlackboard->SetValueAsBool(PathCompletedKey.SelectedKeyName, true);
			UE_LOG(LogTemp, Log, TEXT("BTService_UpdatePathProgress %s: Path completed."), *AIController->GetNameSafe(AIController->GetPawn()));
		}
	}
}

FString UBTService_UpdatePathProgress::GetStaticDescription() const
{
	return FString::Printf(TEXT("Update Path Progress: TargetKey='%s', IdxKey='%s', DoneKey='%s', HasPathKey='%s', ActionKey='%s', ActionTargetKey='%s', Radius=%.1f"),
		*PathTargetLocationKey.SelectedKeyName.ToString(),
		*PathCurrentIndexKey.SelectedKeyName.ToString(),
		*PathCompletedKey.SelectedKeyName.ToString(),
		*HasPathKey.SelectedKeyName.ToString(),
		*CurrentActionTagKey.SelectedKeyName.ToString(),
		*CurrentActionTargetKey.SelectedKeyName.ToString(),
		AcceptanceRadius
	);
}
