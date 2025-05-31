// Copyright Epic Games, Inc. All Rights Reserved.

#include "Planning/RaidPlanData.h"

URaidPlanData::URaidPlanData()
{
	// Constructor logic if needed
}

void URaidPlanData::AddPointToPath(const FRaidPlanPointData& PointData)
{
	CurrentPath.Points.Add(PointData);
}

void URaidPlanData::ClearPath()
{
	CurrentPath.Points.Empty();
}

const TArray<FRaidPlanPointData>& URaidPlanData::GetPathPoints() const
{
	return CurrentPath.Points;
}
