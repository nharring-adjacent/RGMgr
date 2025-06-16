# AI - Utility AI Performance Hotspot in Action Selection

## Justification

The `UUtilityAIComponent::SelectBestAction` function, if it instantiates `UUtilityAIAction` and `UUtilityAIConsideration` objects on every call, can become a significant performance bottleneck. Object instantiation, especially in a high-frequency context like action selection (which might occur every few frames or seconds per AI agent), can lead to memory allocation overhead and garbage collection pressure. The cost of not fixing this is degraded AI performance, potentially causing noticeable hitches or slowdowns, especially with many AI agents.

## Problem Description

The current implementation of `UUtilityAIComponent::SelectBestAction` might involve creating new instances of all potential actions and their associated considerations each time an AI agent needs to decide what to do. This is resource-intensive.

```cpp
// Hypothetical snippet from UUtilityAIComponent.cpp
UUtilityAIAction* UUtilityAIComponent::SelectBestAction()
{
    UUtilityAIAction* BestAction = nullptr;
    float HighestScore = -1.0f;

    for (TSubclassOf<UUtilityAIAction> ActionClass : ActionClasses) // ActionClasses are UClass definitions
    {
        UUtilityAIAction* ActionInstance = NewObject<UUtilityAIAction>(this, ActionClass); // Instantiation
        if (!ActionInstance) continue;

        // ActionInstance would then potentially instantiate its considerations
        float Score = ActionInstance->ScoreAction(this /* Owning AI Component */, GetOwner() /* Controlled Pawn */);

        if (Score > HighestScore)
        {
            HighestScore = Score;
            BestAction = ActionInstance; // Potential issue: what about other instances?
        }
        // else: ActionInstance might be leaked if not handled correctly, or GC'd frequently
    }
    return BestAction; // Only returns one, others are discarded
}
```

## Agent Task

Optimize `UUtilityAIComponent::SelectBestAction` to minimize or eliminate the instantiation of actions and considerations on each call. Pre-instantiate actions and considerations, or use a pooling system.

## Sub-Tasks

- [ ] Analyze `UUtilityAIComponent::SelectBestAction` to confirm the instantiation pattern.
- [ ] Modify `UUtilityAIComponent` to pre-instantiate and store instances of all configured `UUtilityAIAction`s, possibly in a `TArray<UUtilityAIAction*>`.
- [ ] Ensure that `UUtilityAIAction` instances can be reused and reset their internal state if necessary between evaluations.
- [ ] If actions have considerations, ensure these are also pre-instantiated or managed efficiently.
- [ ] Update `SelectBestAction` to iterate over the pre-instantiated actions and score them.
- [ ] Profile the changes to confirm performance improvement and ensure no memory leaks are introduced.
- [ ] Consider if actions need to be stateful; if so, the pre-instantiation approach needs to ensure state is correctly managed or actions are cloned from templates.

## Files to Examine/Modify

- `Source/RaidGame/Private/AI/UtilityAIComponent.cpp`
- `Source/RaidGame/Public/AI/UtilityAIComponent.h`
- `Source/RaidGame/Private/AI/UtilityAIAction.cpp`
- `Source/RaidGame/Public/AI/UtilityAIAction.h`
- `Source/RaidGame/Private/AI/UtilityAIConsideration.cpp`
- `Source/RaidGame/Public/AI/UtilityAIConsideration.h`
