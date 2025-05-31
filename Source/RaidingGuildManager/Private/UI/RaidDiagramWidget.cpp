// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/RaidDiagramWidget.h"
#include "Planning/RaidPlanData.h" // Include the UObject that holds the path
#include "Roster/RosterManager.h"   // For setting the active plan
#include "Engine/GameInstance.h"     // For GetGameInstance to get RosterManager

URaidDiagramWidget::URaidDiagramWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RosterManagerInstance(nullptr) // Initialize RosterManagerInstance
{
	// Create the UObject that will store our path data.
	// This object is owned by the widget.
	PlanData = CreateDefaultSubobject<URaidPlanData>(TEXT("PlanDataObject"));
}

// Example of NativeConstruct if you need to do things when the widget is created in Blueprint
// void URaidDiagramWidget::NativeConstruct()
// {
// 	Super::NativeConstruct();
//
// 	// Example: Setup initial state or bindings if not done in Blueprint
// 	if (PlanData)
// 	{
// 		PlanData->ClearPath(); // Ensure path is clear on widget setup
// 	}
// }

// It's good practice to also get RosterManagerInstance in NativeConstruct
// as GetGameInstance() is more reliably available there than in the constructor.
void URaidDiagramWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		RosterManagerInstance = GI->GetSubsystem<URosterManager>();
		if (!RosterManagerInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("RaidDiagramWidget: RosterManagerInstance is null in NativeConstruct."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RaidDiagramWidget: GameInstance is null in NativeConstruct."));
	}

	// Ensure path is clear on widget setup
	if (PlanData)
	{
		PlanData->ClearPath();
	}
}


void URaidDiagramWidget::StartNewPath(const FVector2D& InitialPoint, FGameplayTag Action, const FString& TargetID)
{
	if (PlanData)
	{
		PlanData->ClearPath();

		FRaidPlanPointData PointData;
		PointData.Location = InitialPoint;
		PointData.ActionTag = Action;
		PointData.TargetIdentifier = TargetID;
		PlanData->AddPointToPath(PointData);

		UE_LOG(LogTemp, Log, TEXT("RaidDiagramWidget: Started new path at X=%.2f, Y=%.2f. Action: %s, TargetID: %s"),
			InitialPoint.X, InitialPoint.Y, *Action.ToString(), *TargetID);

		// Request a repaint if drawing is handled in OnPaint
		// MarkDirty(); // This forces a repaint, useful if your OnPaint depends on this data.
	}
}

void URaidDiagramWidget::HandleMouseDown(const FVector2D& LocalPosition, FGameplayTag Action, const FString& TargetID)
{
	if (PlanData)
	{
		FRaidPlanPointData PointData;
		PointData.Location = LocalPosition;
		PointData.ActionTag = Action;
		PointData.TargetIdentifier = TargetID;
		PlanData->AddPointToPath(PointData);

		UE_LOG(LogTemp, Log, TEXT("RaidDiagramWidget: Added point to path at X=%.2f, Y=%.2f. Action: %s, TargetID: %s. Total points: %d"),
			LocalPosition.X, LocalPosition.Y, *Action.ToString(), *TargetID, PlanData->GetPathPoints().Num());

		// Request a repaint if drawing is handled in OnPaint
		// MarkDirty();
	}
}

void URaidDiagramWidget::FinalizePath()
{
	if (PlanData)
	{
		const TArray<FRaidPlanPointData>& PathPoints = PlanData->GetPathPoints();
		UE_LOG(LogTemp, Log, TEXT("RaidDiagramWidget: Path finalized with %d points."), PathPoints.Num());
		for (int32 i = 0; i < PathPoints.Num(); ++i)
		{
			const FRaidPlanPointData& Point = PathPoints[i];
			UE_LOG(LogTemp, Log, TEXT("RaidDiagramWidget: Finalized Path Point %d: Location=%s, Action=%s, TargetID=%s"),
				i, *Point.Location.ToString(), *Point.ActionTag.ToString(), *Point.TargetIdentifier);
		}

		if (RosterManagerInstance)
		{
			RosterManagerInstance->SetActiveRaidPlan(PlanData);
			UE_LOG(LogTemp, Log, TEXT("RaidDiagramWidget: ActiveRaidPlan set in RosterManager."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RaidDiagramWidget: RosterManagerInstance is null, cannot set ActiveRaidPlan."));
			// Attempt to re-acquire, though this suggests an issue with initialization order or widget lifecycle
			UGameInstance* GI = GetGameInstance();
			if (GI) RosterManagerInstance = GI->GetSubsystem<URosterManager>();
			if (RosterManagerInstance)
			{
				RosterManagerInstance->SetActiveRaidPlan(PlanData);
				UE_LOG(LogTemp, Log, TEXT("RaidDiagramWidget: Re-acquired RosterManagerInstance and set ActiveRaidPlan."));
			}
		}

		// Future: Broadcast an event, e.g.:
		// OnPathFinalized.Broadcast(PathPoints);
	}
}
