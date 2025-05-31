// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BossAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ABossAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABossAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	// Blackboard keys (can be shared or specific)
	static const FName TargetActorKeyName;
	static const FName SelfActorKeyName;
};
