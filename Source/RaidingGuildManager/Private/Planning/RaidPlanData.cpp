// Copyright Epic Games, Inc. All Rights Reserved.

#include "Planning/RaidPlanData.h"

URaidPlanData::URaidPlanData()
{
	// Initialize NamedPaths as an empty map
	NamedPaths.Empty();
}

void URaidPlanData::AddPointToPath(const FString& CharacterOrRoleID, const FRaidPlanPointData& PointData)
{
	FRaidPlanPath& Path = NamedPaths.FindOrAdd(CharacterOrRoleID);
	Path.Points.Add(PointData);
}

void URaidPlanData::ClearPathForCharacter(const FString& CharacterOrRoleID)
{
	if (FRaidPlanPath* Path = NamedPaths.Find(CharacterOrRoleID))
	{
		Path->Points.Empty();
	}
	// Optionally, remove the key if the path is empty and that's desired behavior
	// NamedPaths.Remove(CharacterOrRoleID);
}

void URaidPlanData::ClearAllPaths()
{
	NamedPaths.Empty();
}

const TArray<FRaidPlanPointData>* URaidPlanData::GetPathPointsForCharacter(const FString& CharacterOrRoleID) const
{
	return (const TArray<FRaidPlanPointData>*)(NamedPaths.Find(CharacterOrRoleID) ? &NamedPaths.Find(CharacterOrRoleID)->Points : nullptr);
}

const TMap<FString, FRaidPlanPath>& URaidPlanData::GetAllNamedPaths() const
{
	return NamedPaths;
}
