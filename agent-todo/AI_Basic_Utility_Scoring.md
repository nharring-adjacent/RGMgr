# AI - Basic Utility AI Scoring Lacks Nuance

## Justification

The current utility AI scoring mechanism, if it relies on a simple average of consideration scores or a default score when no considerations are present, might lack the nuance required for complex AI decision-making. A simple average treats all considerations equally, which may not be desired. A default score for actions without considerations makes it difficult to rank them against actions that do have them. The cost of not fixing this is AI behavior that may appear simplistic, predictable, or suboptimal because it cannot make fine-grained distinctions between choices.

## Problem Description

If an action's utility is calculated by simply averaging its consideration scores, it doesn't allow for more sophisticated aggregation methods like weighted averaging, prioritization, or conditional logic. Furthermore, if an action has no considerations, its score might default to a fixed value (e.g., 0.5), making it hard to integrate into the overall selection process meaningfully.

```cpp
// Hypothetical snippet from UUtilityAIAction.cpp
float UUtilityAIAction::ScoreAction(UUtilityAIComponent* OwnerComp, AActor* ControlledPawn)
{
    if (Considerations.Num() == 0)
    {
        return DefaultScore; // e.g., 0.5 - How does this compare to actions with considerations?
    }

    float TotalScore = 0.0f;
    for (UUtilityAIConsideration* Consideration : Considerations)
    {
        if (Consideration)
        {
            TotalScore += Consideration->Score(OwnerComp, ControlledPawn);
        }
    }
    return TotalScore / Considerations.Num(); // Simple average
}
```

## Agent Task

Enhance the utility AI scoring mechanism to allow for more nuanced calculations. This could include support for weighted considerations, different aggregation methods (e.g., multiplication, highest score), and a more flexible way to handle actions with no considerations.

## Sub-Tasks

- [ ] Review the current scoring logic in `UUtilityAIAction::ScoreAction`.
- [ ] Introduce a mechanism for weighting considerations (e.g., a `Weight` property in `UUtilityAIConsideration` or a separate configuration in `UUtilityAIAction`).
- [ ] Allow `UUtilityAIAction` to define its aggregation method (e.g., via an enum: Average, WeightedAverage, Multiply, HighestWins).
- [ ] Implement the different aggregation methods in `UUtilityAIAction::ScoreAction`.
- [ ] Re-evaluate how actions without considerations are scored. Perhaps they should have a configurable 'base utility' or be explicitly designed to be chosen only when no other considered actions are viable.
- [ ] Update any existing actions and considerations to use the new scoring features appropriately.
- [ ] Add data validation to ensure weights and scoring configurations are sensible (e.g., weights are not negative if that's not desired).

## Files to Examine/Modify

- `Source/RaidGame/Private/AI/UtilityAIAction.cpp`
- `Source/RaidGame/Public/AI/UtilityAIAction.h`
- `Source/RaidGame/Private/AI/UtilityAIConsideration.cpp`
- `Source/RaidGame/Public/AI/UtilityAIConsideration.h`
- Data assets or blueprints where actions and considerations are defined (if applicable).
