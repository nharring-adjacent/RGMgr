// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h" // Added for FGameplayTag
#include "Planning/RaidPlanDataTypes.h" // Include the structs we just defined
#include "RaidPlanData.generated.h"

/**
 * UObject to store and manage a single raid plan, potentially with multiple named paths.
 */
UCLASS(BlueprintType, Blueprintable) // Blueprintable to allow Blueprint subclassing if needed
class RAIDINGGUILDMANAGER_API URaidPlanData : public UObject
{
	GENERATED_BODY()

public:
	URaidPlanData();

	// Stores paths for different characters or roles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raid Plan")
	TMap<FString, FRaidPlanPath> NamedPaths;

	// Adds a point to the path for a specific character or role
	UFUNCTION(BlueprintCallable, Category = "Raid Plan")
	void AddPointToPath(const FString& CharacterOrRoleID, const FRaidPlanPointData& PointData);

	// Clears the path for a specific character or role
	UFUNCTION(BlueprintCallable, Category = "Raid Plan")
	void ClearPathForCharacter(const FString& CharacterOrRoleID);

	// Clears all paths
	UFUNCTION(BlueprintCallable, Category = "Raid Plan")
	void ClearAllPaths();

	// Returns all points in the path for a specific character or role
	// Returns a pointer to the array, or nullptr if the ID is not found.
	UFUNCTION(BlueprintPure, Category = "Raid Plan")
	const TArray<FRaidPlanPointData>* GetPathPointsForCharacter(const FString& CharacterOrRoleID) const;

	// Returns all named paths
	UFUNCTION(BlueprintPure, Category = "Raid Plan") // Changed to BlueprintPure as it's a getter
	const TMap<FString, FRaidPlanPath>& GetAllNamedPaths() const;
};
