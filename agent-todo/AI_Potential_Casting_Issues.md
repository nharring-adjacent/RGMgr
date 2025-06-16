# AI - Potential Casting Issues in Health Consideration

## Justification

The `UConsideration_HealthPercentage` class, if it assumes that the Pawn it's evaluating is always an `ARGMCharacterBase` (or a specific subclass), introduces a hard dependency and a potential point of failure. If this consideration is ever used with an AI controlling a different type of Pawn (e.g., a vehicle, a destructible object with health but not an `ARGMCharacterBase`), the cast will fail, leading to incorrect behavior (likely a zero score or a crash if the pointer is dereferenced without checking). The cost of not fixing this is reduced flexibility of the AI system and potential runtime errors if new Pawn types are introduced or configurations are changed.

## Problem Description

Inside `UConsideration_HealthPercentage::ScoreConsideration`, there might be a direct cast of the `ControlledPawn` to `ARGMCharacterBase` to access its health attributes or health component.

```cpp
// Hypothetical snippet from UConsideration_HealthPercentage.cpp
float UConsideration_HealthPercentage::Score(UUtilityAIComponent* OwnerComp, AActor* ControlledPawn)
{
    ARGMCharacterBase* Character = Cast<ARGMCharacterBase>(ControlledPawn);
    if (Character)
    {
        // Assume ARGMCharacterBase has URGMAttributeComponent and GetHealthAttribute()
        URGMAttributeComponent* AttributeComp = Character->GetAttributeComponent(); // Or similar accessor
        if (AttributeComp)
        {
            float Health = AttributeComp->GetHealth();
            float MaxHealth = AttributeComp->GetMaxHealth();
            if (MaxHealth > 0)
            {
                return Health / MaxHealth;
            }
        }
    }
    // What happens if the cast fails? Returns 0? This might be misleading.
    // Or worse, if Character is used without a null check after a failed cast (less likely with Cast<>, but possible with other cast types or direct access).
    return 0.0f;
}
```

## Agent Task

Refactor `UConsideration_HealthPercentage` to safely access health information from any Pawn that supports it, without relying on a specific class cast. This typically involves using interfaces or components.

## Sub-Tasks

- [ ] Define a C++ interface (e.g., `IHealthInterface`) with methods like `GetHealth()` and `GetMaxHealth()`.
- [ ] Modify `ARGMCharacterBase` (and any other relevant character/pawn classes that have health) to implement this interface.
- [ ] Update `UConsideration_HealthPercentage` to attempt to retrieve the `IHealthInterface` from the `ControlledPawn`.
- [ ] If the interface is present, use its methods to get health information.
- [ ] If the interface is not present, the consideration should score appropriately (e.g., return 0 or a specific "not applicable" score).
- [ ] Ensure existing Pawns that should have health correctly implement the new interface.
- [ ] Test with AI controlling `ARGMCharacterBase` and, if possible, a different Pawn type that does *not* have health to ensure the fallback works correctly.

## Files to Examine/Modify

- `Source/RaidGame/Private/AI/Considerations/Consideration_HealthPercentage.cpp`
- `Source/RaidGame/Public/AI/Considerations/Consideration_HealthPercentage.h`
- `Source/RaidGame/Public/Character/RGMCharacterBase.h`
- `Source/RaidGame/Private/Character/RGMCharacterBase.cpp`
- Potentially create `Source/RaidGame/Public/Interfaces/HealthInterface.h`
- Any other Pawn classes that have health and might be controlled by AI using this consideration.
