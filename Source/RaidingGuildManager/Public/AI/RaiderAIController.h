// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RaiderAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ARaiderAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARaiderAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	// Blackboard keys
	static const FName TargetActorKeyName;
	static const FName SelfActorKeyName;
	static const FName PathTargetLocationKeyName;
	static const FName HasPathKeyName;
	static const FName PathCurrentIndexKeyName;
	static const FName PathCompletedKeyName;
	// Voice Command Blackboard Keys
	static const FName VoiceCommandTagKeyName;
	static const FName VoiceCommandTargetKeyName;
	static const FName VoiceCommandLocationKeyName;
	// New Blackboard keys for path point actions
	static const FName CurrentActionTagKeyName;
	static const FName CurrentActionTargetKeyName;


	void SetPathToFollow(const TArray<FRaidPlanPointData>& PathPoints); // Updated parameter type
	const TArray<FRaidPlanPointData>& GetCurrentPathPoints() const; // Updated return type
	void ExecuteVoiceCommand(FGameplayTag CommandTag, UObject* Target, const FVector& Location);
	void ResolveAndSetTarget(const FString& TargetID, FName BBKeyName);


private:
	TArray<FRaidPlanPointData> CurrentRaidPathPoints; // Updated type
};
