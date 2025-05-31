// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Planning/RaidPlanDataTypes.h" // Include the structs we just defined
#include "RaidPlanData.generated.h"

/**
 * UObject to store and manage a single raid plan path.
 */
UCLASS(BlueprintType, Blueprintable) // Blueprintable to allow Blueprint subclassing if needed
class RAIDINGGUILDMANAGER_API URaidPlanData : public UObject
{
	GENERATED_BODY()

public:
	URaidPlanData();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raid Plan")
	FRaidPlanPath CurrentPath; // This now uses FRaidPlanPointData internally

	UFUNCTION(BlueprintCallable, Category = "Raid Plan")
	void AddPointToPath(const FRaidPlanPointData& PointData);

	UFUNCTION(BlueprintCallable, Category = "Raid Plan")
	void ClearPath();

	UFUNCTION(BlueprintPure, Category = "Raid Plan")
	const TArray<FRaidPlanPointData>& GetPathPoints() const;
};
