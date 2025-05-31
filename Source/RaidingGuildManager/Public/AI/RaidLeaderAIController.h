// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/RaiderAIController.h" // Include for ARaiderAIController
#include "RaidLeaderAIController.generated.h"

class URaidPlanData;
class ARaiderAIController; // Forward declaration still okay if only used in TMap value

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ARaidLeaderAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARaidLeaderAIController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raid Leader")
	URaidPlanData* CurrentRaidPlan;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raid Leader")
	TMap<FString, ARaiderAIController*> ManagedRaiders; // Map to keep track of raiders

	UFUNCTION(BlueprintCallable, Category = "Raid Leader")
	void SetCurrentRaidPlan(URaidPlanData* NewPlan);

	UFUNCTION(BlueprintCallable, Category = "Raid Leader")
	void DistributePlanToSquad();
};
