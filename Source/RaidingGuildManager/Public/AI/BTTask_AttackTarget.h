// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AttackTarget.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UBTTask_AttackTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_AttackTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	// Store the AbilitySystemComponent of the attacker for handling ability completion
	UPROPERTY()
	UAbilitySystemComponent* CachedAbilitySystemComponent;

	// Handle to the GameplayAbility that was activated
	FGameplayAbilitySpecHandle ActivatedAbilityHandle;

	// Delegate handle for when the ability ends
	FDelegateHandle OnAbilityEndedDelegateHandle;

	void OnAbilityEnded(const FGameplayAbilitySpecHandle& Handle, bool bWasCancelled);
};
