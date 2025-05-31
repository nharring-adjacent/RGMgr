// In Source/RaidingGuildManager/Public/AI/BossAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/BossStageStruct.h" // For EStageTriggerCondition
#include "BossAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;
class UBossDataAsset; // Forward declare
class ABossCharacter; // Forward declare
class UGameplayAbility; // For ability management
class UAbilitySystemComponent; // For ability management

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
    virtual void Tick(float DeltaSeconds) override; // For timer-based stage transitions

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBehaviorTreeComponent* BehaviorTreeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

    // This will be the primary BT if not overridden by stage-specific BTs
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* DefaultBehaviorTreeAsset;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Boss Config")
    UBossDataAsset* BossData; // Assign this in the editor or when spawning the boss

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|Boss State")
    int32 CurrentStageIndex;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI|Boss State")
    FBossStageStruct CurrentStageData; // A copy of the current stage's data

    // Blackboard keys
    static const FName TargetActorKeyName;
    static const FName SelfActorKeyName;
    static const FName CurrentStageNameKeyName; // New BB Key for current stage name

protected:
    UPROPERTY()
    ABossCharacter* PossessedBossCharacter; // Cache the possessed character

    UPROPERTY()
    UAbilitySystemComponent* BossAbilitySystemComponent; // Cache ASC

    float FightStartTime; // To track time for timer-based triggers
    bool bStageLogicInitialized;

    void InitializeBossLogic();
    void TransitionToStage(int32 NewStageIndex);
    void UpdateAbilitiesForCurrentStage();
    void CheckStageTransitionConditions(float DeltaSeconds);

    // Helper to grant an ability if the class is loaded
    FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1);
    void ClearAbilitiesBySpecHandles(const TArray<FGameplayAbilitySpecHandle>& SpecHandles);

    TArray<FGameplayAbilitySpecHandle> CurrentStageAbilitySpecHandles;
};
