// In Source/RaidingGuildManager/Private/AI/BossAIController.cpp

#include "AI/BossAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h" // For UBehaviorTree
#include "Characters/BossCharacter.h"
#include "Data/BossDataAsset.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h" // For UGameplayAbility
#include "GameplayTagsManager.h" // For resolving tags to abilities if needed (advanced)
// It's assumed UGameplayAbility subclasses are loaded or can be resolved from FGameplayTag.
// For simplicity, we'll assume FBossAbilityStruct::AbilityTag can be used to find/load UGameplayAbility class.
// A more robust system would use TSoftClassPtr<UGameplayAbility> in FBossAbilityStruct.

// Define static const FName members
const FName ABossAIController::TargetActorKeyName = FName("TargetActor");
const FName ABossAIController::SelfActorKeyName = FName("SelfActor");
const FName ABossAIController::CurrentStageNameKeyName = FName("CurrentStageName");


ABossAIController::ABossAIController()
{
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    CurrentStageIndex = -1;
    FightStartTime = 0.0f;
    bStageLogicInitialized = false;
    PossessedBossCharacter = nullptr;
    BossAbilitySystemComponent = nullptr;
}

void ABossAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (InPawn == nullptr)
    {
        return;
    }

    PossessedBossCharacter = Cast<ABossCharacter>(InPawn);
    if (PossessedBossCharacter)
    {
        BossAbilitySystemComponent = PossessedBossCharacter->GetAbilitySystemComponent();
        if (BossAbilitySystemComponent)
        {
            // Standard attribute/ability init (might be redundant if BossData handles all)
            PossessedBossCharacter->InitializeAttributes();
            // PossessedBossCharacter->GiveDefaultAbilities(); // This might be overridden by BossData default abilities

            InitializeBossLogic(); // Setup based on BossData
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BossAIController %s: PossessedBossCharacter %s has no AbilitySystemComponent!"), *GetNameSafe(this), *InPawn->GetName());
        }

        // Behavior Tree setup - this will now use DefaultBehaviorTreeAsset or stage-specific one
        // The actual running of BT will happen in TransitionToStage or InitializeBossLogic
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BossAIController::OnPossess: InPawn is not an ABossCharacter. Pawn: %s"), *GetNameSafe(InPawn));
    }
}

void ABossAIController::InitializeBossLogic()
{
    if (!PossessedBossCharacter || !BossData || !BossAbilitySystemComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("BossAIController %s: Cannot initialize boss logic. Missing Character, BossData, or ASC."), *GetNameSafe(this));
        bStageLogicInitialized = false;
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Initializing logic with BossData %s"), *GetNameSafe(this), *BossData->GetName());

    // Clear any previously granted stage abilities
    ClearAbilitiesBySpecHandles(CurrentStageAbilitySpecHandles);
    CurrentStageAbilitySpecHandles.Empty();

    // Grant default abilities from BossData
    for (const FBossAbilityStruct& AbilityInfo : BossData->DefaultAbilities)
    {
        // This is a placeholder: you need a way to get TSubclassOf<UGameplayAbility> from FGameplayTag
        // For example, using an AssetManager or a DataTable that maps tags to ability classes.
        // For now, assume AbilityInfo.AbilityTag.ToString() can be resolved to a loaded UClass path (very unlikely for complex projects)
        // Or, more realistically, FBossAbilityStruct would store TSoftClassPtr<UGameplayAbility>
        UClass* AbilityClass = FindObject<UClass>(ANY_PACKAGE, *AbilityInfo.AbilityTag.GetTagName().ToString());
        if (TSubclassOf<UGameplayAbility> GASAbilityClass = AbilityClass)
        {
            GrantAbility(GASAbilityClass);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not find GameplayAbility class for tag: %s in BossData default abilities"), *AbilityInfo.AbilityTag.ToString());
        }
    }


    // Determine initial stage (usually the first one, index 0)
    if (BossData->Stages.Num() > 0)
    {
        TransitionToStage(0);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("BossAIController %s: No stages defined in BossData. Running default behavior."), *GetNameSafe(this));
        // If no stages, run default BT if available
        if (BlackboardComponent && DefaultBehaviorTreeAsset && DefaultBehaviorTreeAsset->BlackboardAsset)
        {
            InitializeBlackboard(*BlackboardComponent, *DefaultBehaviorTreeAsset->BlackboardAsset);
            BlackboardComponent->SetValueAsObject(SelfActorKeyName, PossessedBossCharacter);
            BlackboardComponent->SetValueAsString(CurrentStageNameKeyName, TEXT("Default"));
            RunBehaviorTree(DefaultBehaviorTreeAsset);
        }
    }
    FightStartTime = GetWorld()->GetTimeSeconds();
    bStageLogicInitialized = true;
}

void ABossAIController::TransitionToStage(int32 NewStageIndex)
{
    if (!PossessedBossCharacter || !BossData || !BossAbilitySystemComponent || !BossData->Stages.IsValidIndex(NewStageIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("BossAIController %s: Cannot transition to stage %d. Invalid conditions or index."), *GetNameSafe(this), NewStageIndex);
        return;
    }

    // Optional: Logic before leaving current stage (e.g., clean up effects)
    // ...

    CurrentStageIndex = NewStageIndex;
    CurrentStageData = BossData->Stages[CurrentStageIndex]; // Make a copy

    UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Transitioning to stage %d: %s"), *GetNameSafe(this), CurrentStageIndex, *CurrentStageData.StageName);

    // Update Abilities
    UpdateAbilitiesForCurrentStage();

    // Update Behavior Tree (if stage specifies one, otherwise use default or keep current)
    // For simplicity, we'll just set a blackboard key. A more complex system could change the BT asset.
    if (BlackboardComponent)
    {
        // Ensure Blackboard is initialized if not already (e.g. if no default BT was run)
        if (!BlackboardComponent->GetBlackboardAsset() && DefaultBehaviorTreeAsset && DefaultBehaviorTreeAsset->BlackboardAsset) {
             InitializeBlackboard(*BlackboardComponent, *DefaultBehaviorTreeAsset->BlackboardAsset);
        } else if (!BlackboardComponent->GetBlackboardAsset() && BossData->Stages.IsValidIndex(NewStageIndex)) {
            // Attempt to find a BT from stage data if possible, or use a generic one
            // This part needs a proper design for how stages link to BTs.
            // For now, assume DefaultBehaviorTreeAsset is the main one.
            if (DefaultBehaviorTreeAsset && DefaultBehaviorTreeAsset->BlackboardAsset) {
                InitializeBlackboard(*BlackboardComponent, *DefaultBehaviorTreeAsset->BlackboardAsset);
            } else {
                 UE_LOG(LogTemp, Warning, TEXT("BossAIController %s: No valid BlackboardAsset to initialize for stage transition!"), *GetNameSafe(this));
                 return; // Cannot proceed without a blackboard
            }
        }


        BlackboardComponent->SetValueAsObject(SelfActorKeyName, PossessedBossCharacter);
        BlackboardComponent->SetValueAsString(CurrentStageNameKeyName, CurrentStageData.StageName);
        UE_LOG(LogTemp, Log, TEXT("Blackboard Key '%s' set to '%s'"), *CurrentStageNameKeyName.ToString(), *CurrentStageData.StageName);

        // If this is the first stage transition and BT hasn't started, or if you want to restart BT on each stage:
        // For now, we assume one main BT (DefaultBehaviorTreeAsset) that reacts to CurrentStageNameKeyName
        if (DefaultBehaviorTreeAsset && (!BehaviorTreeComponent->IsRunning() || BehaviorTreeComponent->GetCurrentTree() != DefaultBehaviorTreeAsset) )
        {
             RunBehaviorTree(DefaultBehaviorTreeAsset);
             UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Started Behavior Tree %s for stage %s"), *GetNameSafe(this), *DefaultBehaviorTreeAsset->GetName(), *CurrentStageData.StageName);
        } else if (!DefaultBehaviorTreeAsset) {
            UE_LOG(LogTemp, Warning, TEXT("BossAIController %s: DefaultBehaviorTreeAsset is not set! Cannot run behavior tree."), *GetNameSafe(this));
        }
    }

    // Trigger environment changes (conceptual)
    if (CurrentStageData.EnvironmentChangeTag.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Stage %s requests environment change: %s"), *GetNameSafe(this), *CurrentStageData.StageName, *CurrentStageData.EnvironmentChangeTag.ToString());
        // Implement actual environment change logic here (e.g., fire an event, call a subsystem)
        // UGameplayCueManager::ExecuteGameplayCue_NonInstigated(PossessedBossCharacter, CurrentStageData.EnvironmentChangeTag, EGameplayCueEvent::Type::OnActive);
    }

    // Reset timer for next timer-based trigger if current trigger was timer-based
    if (CurrentStageData.TriggerCondition == EStageTriggerCondition::Timer)
    {
        FightStartTime = GetWorld()->GetTimeSeconds(); // Reset timer relative to this stage start
    }
}

void ABossAIController::UpdateAbilitiesForCurrentStage()
{
    if (!BossAbilitySystemComponent || !PossessedBossCharacter) return;

    // Clear abilities from previous stage
    ClearAbilitiesBySpecHandles(CurrentStageAbilitySpecHandles);
    CurrentStageAbilitySpecHandles.Empty();

    // Grant abilities for the current stage
    for (const FBossAbilityStruct& AbilityInfo : CurrentStageData.Abilities)
    {
        // Placeholder: Resolve FGameplayTag to TSubclassOf<UGameplayAbility>
        // This is a critical part that needs a proper implementation strategy.
        // E.g., using an AssetManager, a DataTable mapping tags to ability classes,
        // or changing FBossAbilityStruct to use TSoftClassPtr<UGameplayAbility>.
        UClass* AbilityClass = FindObject<UClass>(ANY_PACKAGE, *AbilityInfo.AbilityTag.GetTagName().ToString());
        if (TSubclassOf<UGameplayAbility> GASAbilityClass = AbilityClass)
        {
            FGameplayAbilitySpecHandle SpecHandle = GrantAbility(GASAbilityClass);
            if(SpecHandle.IsValid())
            {
                CurrentStageAbilitySpecHandles.Add(SpecHandle);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not find GameplayAbility class for tag: %s in stage %s"), *AbilityInfo.AbilityTag.ToString(), *CurrentStageData.StageName);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Updated abilities for stage %s. Granted %d abilities."), *GetNameSafe(this), *CurrentStageData.StageName, CurrentStageAbilitySpecHandles.Num());
}

void ABossAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bStageLogicInitialized || !PossessedBossCharacter || !BossData || CurrentStageIndex < 0)
    {
        return; // Not ready or no stages to manage
    }

    CheckStageTransitionConditions(DeltaSeconds);
}

void ABossAIController::CheckStageTransitionConditions(float DeltaSeconds)
{
    if (!BossData || !PossessedBossCharacter || !BossAbilitySystemComponent) return;

    // Iterate through all stages to check if any transition should occur from the current stage.
    // This allows for non-linear stage progression if designed that way,
    // but typically you'd look for the "next" stage.
    // For simplicity, we'll check for transitions to stages with a higher index first.

    // Consider if the current stage is repeatable. If so, conditions might trigger a re-entry.
    // For now, we only transition to stages with a higher index to avoid infinite loops on simple setups.
    // A more robust system would handle specific exit conditions for the current stage too.

    for (int32 PotentialNextStageIndex = 0; PotentialNextStageIndex < BossData->Stages.Num(); ++PotentialNextStageIndex)
    {
        if (PotentialNextStageIndex == CurrentStageIndex && !BossData->Stages[CurrentStageIndex].bRepeatable)
        {
            // If it's the current stage and it's not repeatable, don't try to transition to itself.
            // If it IS repeatable, its own conditions might re-trigger it (e.g. heal above a health threshold then drop below again)
            // This simple loop might not perfectly handle re-triggering repeatable stages if they are not the "next" one.
            continue;
        }
        // If not repeatable, we usually only want to move "forward" in stages.
        // However, a specific game design might allow going back to an earlier stage if it IS repeatable.
        // For now: only transition if PotentialNextStageIndex is different, or if it's the same stage and it's repeatable.
        if (PotentialNextStageIndex != CurrentStageIndex || (PotentialNextStageIndex == CurrentStageIndex && BossData->Stages[CurrentStageIndex].bRepeatable) )
        {
            const FBossStageStruct& PotentialStage = BossData->Stages[PotentialNextStageIndex];
            bool bConditionMet = false;

            if (PotentialStage.TriggerCondition == EStageTriggerCondition::HealthPercentage)
            {
                // Assuming Health attribute is on PossessedBossCharacter's AttributeSet
                // And that the ASC is valid.
                // THIS IS A PLACEHOLDER FOR YOUR ACTUAL ATTRIBUTE ACCESS:
                // Replace BossAbilitySystemComponent->GetHealthAttribute() and BossAbilitySystemComponent->GetMaxHealthAttribute()
                // with the correct way to get these values from your attribute set.
                // For example, if you have a URGMAttributeSet:
                // const URGMAttributeSet* AttrSet = PossessedBossCharacter->GetAttributeSet();
                // if (AttrSet && AttrSet->GetMaxHealth() > 0) {
                //    if (AttrSet->GetHealth() <= AttrSet->GetMaxHealth() * PotentialStage.TriggerHealthPercentage) { ... }
                // }
                // For now, using a generic (likely non-compiling without engine changes or specific ASC subclass) way:
                // UAttributeSet* BaseAttributeSet = BossAbilitySystemComponent->GetSpawnedAttributes_Mutable()[0]; // Highly unsafe, just for placeholder
                // float Health = BaseAttributeSet->GetAttributeValue(FGameplayAttribute::FindChecked(TEXT("YourGame.YourAttributeSet.Health")));
                // float MaxHealth = BaseAttributeSet->GetAttributeValue(FGameplayAttribute::FindChecked(TEXT("YourGame.YourAttributeSet.MaxHealth")));
                // For this example, I will assume your AbilitySystemComponent class has GetHealthAttribute() and GetMaxHealthAttribute()
                // which return FGameplayAttribute. If not, this part WILL NOT COMPILE and needs to be adapted.
                // To make this example runnable without knowing your exact attribute set, we'll log a warning and skip.
                UE_LOG(LogTemp, Warning, TEXT("Health check for stage transition needs to be implemented with your specific AttributeSet accessors."));
                // Example of what it might look like if you have direct FGameplayAttribute members in your ASC or AttributeSet:
                // float CurrentHealth = BossAbilitySystemComponent->GetNumericAttribute(URGMAttributeSet::GetHealthAttribute());
                // float CurrentMaxHealth = BossAbilitySystemComponent->GetNumericAttribute(URGMAttributeSet::GetMaxHealthAttribute());
                // if (CurrentMaxHealth > 0 && CurrentHealth <= CurrentMaxHealth * PotentialStage.TriggerHealthPercentage)
                // {
                //    bConditionMet = true; // Placeholder
                // }
            }
            else if (PotentialStage.TriggerCondition == EStageTriggerCondition::Timer)
            {
                // Timer relative to the start of the *current* stage if this is the stage's own timer trigger,
                // or relative to overall fight start if it's for a different stage.
                // The current FightStartTime is reset when a timer-triggered stage begins.
                float CurrentTimeInFightOrStage = GetWorld()->GetTimeSeconds() - FightStartTime;
                if (CurrentTimeInFightOrStage >= PotentialStage.TriggerTimerSeconds)
                {
                    bConditionMet = true;
                }
            }

            if (bConditionMet)
            {
                if (PotentialNextStageIndex == CurrentStageIndex && PotentialStage.bRepeatable) {
                    // Re-triggering the same repeatable stage.
                    // We might want to "reset" the stage in some way.
                    UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Re-triggering repeatable stage %d: %s"), *GetNameSafe(this), PotentialNextStageIndex, *PotentialStage.StageName);
                    TransitionToStage(PotentialNextStageIndex); // Re-run transition logic (abilities, BT, etc.)
                    return; // Stop checking further stages for this tick
                } else if (PotentialNextStageIndex != CurrentStageIndex) {
                    UE_LOG(LogTemp, Log, TEXT("BossAIController %s: Condition met for stage %d: %s. Transitioning."), *GetNameSafe(this), PotentialNextStageIndex, *PotentialStage.StageName);
                    TransitionToStage(PotentialNextStageIndex);
                    return; // Stop checking further stages for this tick
                }
            }
        }
    }
}


FGameplayAbilitySpecHandle ABossAIController::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level)
{
    if (!BossAbilitySystemComponent || !AbilityClass) return FGameplayAbilitySpecHandle();

    FGameplayAbilitySpec AbilitySpec(AbilityClass, Level);
    return BossAbilitySystemComponent->GiveAbility(AbilitySpec);
}

void ABossAIController::ClearAbilitiesBySpecHandles(const TArray<FGameplayAbilitySpecHandle>& SpecHandles)
{
    if (!BossAbilitySystemComponent) return;

    for (const FGameplayAbilitySpecHandle& SpecHandle : SpecHandles)
    {
        BossAbilitySystemComponent->ClearAbility(SpecHandle);
    }
}
