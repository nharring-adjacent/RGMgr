# Ability - Unclear Targeting Responsibility in Basic Attack

## Justification

`UGA_BasicAttack` having placeholder targeting logic or an unclear division of responsibility with Behavior Tree (BT) tasks for targeting makes the system confusing and hard to maintain. If the ability itself tries to find targets, it might conflict with or duplicate efforts from AI logic (like BT tasks) that are also supposed to select targets. This can lead to bugs where the wrong target is attacked, or where targeting behavior is inconsistent. The cost of not fixing this is increased difficulty in debugging targeting issues, potential for redundant or conflicting logic, and a less flexible ability system.

## Problem Description

The `UGA_BasicAttack::ActivateAbility` method might contain rudimentary targeting (e.g., find nearest enemy) or have comments like "// TODO: Implement proper targeting". If AI agents use BTs, there's often a `UBTTask_FindTarget` or similar node that stores the target on a Blackboard. The ability should ideally use this Blackboard target rather than performing its own search.

```cpp
// Hypothetical snippet from UGA_BasicAttack.cpp
void UGA_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    AActor* SourceActor = ActorInfo->AvatarActor.Get();
    APawn* SourcePawn = Cast<APawn>(SourceActor);

    // Placeholder targeting:
    // AActor* TargetActor = FindNearestEnemy(SourcePawn); // This logic might be duplicated/conflicting with BT

    // Or, if it's expecting target data from an event:
    const FGameplayAbilityTargetData* TargetDataFromEvent = TriggerEventData ? TriggerEventData->TargetData.Get() : nullptr;

    // If it relies on TargetData being passed in, how is this data sourced for AI?
    // If AI uses a Blackboard, the ability should probably read from there.
    // UBlackboardComponent* BlackboardComp = ActorInfo->OwnerAbilitySystemComponent->GetOwner()->FindComponentByClass<UBlackboardComponent>();
    // AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetEnemy"));


    if (!TargetActor)
    {
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }

    FGameplayAbilityTargetDataHandle TargetDataHandle;
    // Code to package TargetActor into TargetDataHandle

    // ... apply effect ...
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
```

The responsibility for *selecting* a target should ideally rest with the AI's higher-level logic (e.g., Behavior Tree, Utility AI), and the ability should then *act* on that target. Player-initiated targeting is different and usually involves direct input or targeting abilities.

## Agent Task

Clarify and refactor the targeting logic in `UGA_BasicAttack`. For AI-controlled characters, the ability should primarily consume target information established by the AI system (e.g., from a Blackboard). For player characters, it should use standard player targeting mechanisms. Remove any redundant or placeholder targeting searches from the ability itself.

## Sub-Tasks

- [ ] Analyze `UGA_BasicAttack` to identify its current targeting mechanism.
- [ ] Determine how AI agents are intended to select targets (e.g., BT Tasks setting a Blackboard key).
- [ ] If AI uses a Blackboard:
    - [ ] Ensure `UGA_BasicAttack` can access the `UBlackboardComponent` (usually via the `AAIController` owning the `AbilitySystemComponent`).
    - [ ] Modify `UGA_BasicAttack` to retrieve the target from the appropriate Blackboard key.
    - [ ] Define a standard Blackboard key name for the current target (e.g., `BBKEY_CurrentTarget`).
- [ ] If player-controlled, ensure it uses `TargetData` produced by `UAbilityTask_WaitTargetData` or similar, or from `TriggerEventData` if the ability is activated by an event carrying target info.
- [ ] Remove any generic target-searching logic (e.g., "find nearest enemy") from `UGA_BasicAttack` if this responsibility lies elsewhere.
- [ ] Ensure the ability handles cases where no valid target is found (e.g., cancels the ability).
- [ ] Document the expected source of target information for `UGA_BasicAttack`.

## Files to Examine/Modify

- `Source/RaidGame/Private/Abilities/GA_BasicAttack.cpp`
- `Source/RaidGame/Public/Abilities/GA_BasicAttack.h`
- Relevant Behavior Tree Task files (e.g., `UBTTask_Attack.cpp`, `UBTTask_FindTarget.cpp`) that might interact with this ability or set targets.
- AI Controller class(es) (`ARaiderAIController.cpp`, etc.) to see how Blackboard/Targeting is set up.
- Player Controller class(es) if players can use this attack, to see how targeting is initiated.
