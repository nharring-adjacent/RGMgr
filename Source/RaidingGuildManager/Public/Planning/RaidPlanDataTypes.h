// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RaidPlanDataTypes.generated.h"

#include "GameplayTagContainer.h" // Required for FGameplayTag

USTRUCT(BlueprintType)
struct FRaidPlanPointData // Renamed from FRaidPlanPathPoint and expanded
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid Plan")
	FVector2D Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid Plan")
	FGameplayTag ActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid Plan")
	FString TargetIdentifier; // For attack actions, could be a name, role, or "Boss"

	FRaidPlanPointData()
		: Location(FVector2D::ZeroVector)
		, ActionTag(FGameplayTag::RequestGameplayTag(FName("Action.Move"))) // Default to Move
		, TargetIdentifier(TEXT(""))
	{}

	FRaidPlanPointData(const FVector2D& InLocation, const FGameplayTag& InActionTag = FGameplayTag::RequestGameplayTag(FName("Action.Move")), const FString& InTargetIdentifier = TEXT(""))
		: Location(InLocation)
		, ActionTag(InActionTag)
		, TargetIdentifier(InTargetIdentifier)
	{}
};

USTRUCT(BlueprintType)
struct FRaidPlanPath
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid Plan")
	TArray<FRaidPlanPointData> Points; // Changed from TArray<FVector2D>

	// Future properties:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid Plan")
	// FName PathName; // e.g., "TankPath", "DPSGroup1Path"

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raid Plan")
	// int32 AssignedRaiderId; // Or a more complex raider identifier

	FRaidPlanPath()
	{}
};
