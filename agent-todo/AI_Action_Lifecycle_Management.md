# AI - Unclear Lifecycle Management for Persistent Utility AI Actions

## Justification

If `UUtilityAIComponent::SelectBestAction` instantiates actions and the selected action is intended to be persistent (i.e., it executes over time, not instantaneously), its lifecycle management is crucial and potentially problematic. If the action object is not correctly managed, it could be prematurely garbage collected, or alternatively, stale references could persist. The cost of not fixing this includes potential crashes due to accessing invalid objects, memory leaks if objects are not collected, or incorrect AI behavior if actions terminate unexpectedly or run with outdated data.

## Problem Description

When `SelectBestAction` returns a `UUtilityAIAction` that is meant to run for a duration (e.g., a channeling spell, a patrol behavior), it's unclear who owns this action instance and how its lifetime is tied to its execution. If `SelectBestAction` creates new instances each time, the runner of the action (e.g., a Behavior Tree task) needs to ensure it holds a strong reference and knows when to release it.

```cpp
// Hypothetical snippet from a Behavior Tree Task that runs a Utility AI Action
EBTNodeResult::Type UBTTask_ExecuteUtilityAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UAIController* AIController = OwnerComp.GetAIOwner();
    UUtilityAIComponent* UtilityAIComp = AIController->FindComponentByClass<UUtilityAIComponent>();

    if (UtilityAIComp)
    {
        CurrentAction = UtilityAIComp->SelectBestAction(); // Gets a new instance? Or a pre-existing one?
        if (CurrentAction)
        {
            CurrentAction->BeginExecute(AIController, OwnerComp.GetAIOwner()->GetPawn());
            // If CurrentAction is a new instance, what prevents it from being GC'd if this task finishes
            // but the action is still meant to be logically running?
            // If it's a shared instance, how is its state managed if another AI selects it?
            return EBTNodeResult::InProgress;
        }
    }
    return EBTNodeResult::Failed;
}

void UBTTask_ExecuteUtilityAction::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (CurrentAction && CurrentAction->IsExecuting())
    {
        CurrentAction->Tick(DeltaSeconds);
    }
    else
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // Or Failed
    }
}
```

This becomes more complex if actions are pre-instantiated (as suggested in `AI_Utility_Performance_Hotspot.md`). If a single instance is reused, how can it manage state for an action that needs to run over multiple ticks?

## Agent Task

Clarify and implement robust lifecycle management for `UUtilityAIAction` objects, especially for actions that are not instantaneous. Determine ownership and ensure actions persist as long as they are logically executing and are cleaned up correctly afterward.

## Sub-Tasks

- [ ] Review how `UUtilityAIAction` instances are created and returned by `SelectBestAction`.
- [ ] Determine if actions are intended to be stateful and execute over time.
- [ ] If actions are stateful and run over time:
    - [ ] Option A: `SelectBestAction` returns a *copy* or a *new instance* of the selected action template. The caller (e.g., BT Task) owns this instance and is responsible for its destruction/release.
    - [ ] Option B: `SelectBestAction` returns a *handle* or *ID* to an action, and the `UUtilityAIComponent` manages a pool of active action instances.
    - [ ] Option C: Actions are designed to be stateless, and any required persistent data is stored in the `UUtilityAIComponent` or the AI's Blackboard.
- [ ] Implement `BeginExecute`, `TickAction`, and `EndExecute` (or similar) methods in `UUtilityAIAction`.
- [ ] Ensure that whatever system consumes the action (e.g., Behavior Tree tasks) correctly calls these lifecycle methods.
- [ ] Verify that actions are properly garbage collected or returned to a pool when they are no longer needed.
- [ ] Add clear documentation explaining the lifecycle and ownership model.

## Files to Examine/Modify

- `Source/RaidGame/Private/AI/UtilityAIComponent.cpp`
- `Source/RaidGame/Public/AI/UtilityAIComponent.h`
- `Source/RaidGame/Private/AI/UtilityAIAction.cpp`
- `Source/RaidGame/Public/AI/UtilityAIAction.h`
- Any Behavior Tree Tasks or systems that execute these actions (e.g., `UBTTask_ExecuteUtilityAction.cpp`)
