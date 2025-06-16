# Ability - Incomplete Cooldown and Cost Gameplay Effect Setup in Basic Attack

## Justification

If `UGA_BasicAttack` does not correctly define or apply its Cooldown and Cost Gameplay Effects (GEs), these fundamental mechanics will not function as intended. This can lead to abilities being spammable without resource cost, breaking game balance. Proper setup of these GEs is essential for any ability. The cost of not fixing this is unbalanced gameplay, exploitable mechanics, and an ability that doesn't conform to the game's design for resource management and ability pacing.

## Problem Description

The `UGA_BasicAttack` constructor (or related setup) might be missing the assignment of `CooldownGameplayEffectClass` and `CostGameplayEffectClass`, or the assigned GEs might be misconfigured (e.g., zero duration cooldown, zero cost).

```cpp
// Hypothetical snippet from UGA_BasicAttack.h or .cpp (constructor)
UGA_BasicAttack::UGA_BasicAttack()
{
    // ... other setups ...

    // Option 1: Pointers might be null
    // CooldownGameplayEffectClass = nullptr; // Or not set
    // CostGameplayEffectClass = nullptr;   // Or not set

    // Option 2: Pointing to GEs that are not properly defined
    // CooldownGameplayEffectClass = UGE_BasicAttack_Cooldown::StaticClass();
    // CostGameplayEffectClass = UGE_BasicAttack_Cost::StaticClass();
    // But UGE_BasicAttack_Cooldown might have a duration of 0, or UGE_BasicAttack_Cost might affect no attributes or have 0 magnitude.

    // Correct setup involves assigning UClass references to properly configured GE Blueprints or C++ GE classes.
    // For example, in Blueprint, these would be set directly on the ability's defaults.
    // In C++, it would be:
    // static ConstructorHelpers::FClassFinder<UGameplayEffect> CooldownGEFinder(TEXT("Blueprint'/Game/Abilities/GE_DefaultCooldown.GE_DefaultCooldown_C'"));
    // if (CooldownGEFinder.Succeeded()) { CooldownGameplayEffectClass = CooldownGEFinder.Class; }
}
```
When the ability commits, `CommitAbility` (or `CommitAbilityCooldown` and `CommitAbilityCost` separately) applies these GEs. If they are not set or are invalid, the commit will succeed without imposing a cooldown or cost.

## Agent Task

Ensure that `UGA_BasicAttack` has correctly configured and assigned `CooldownGameplayEffectClass` and `CostGameplayEffectClass`. Verify that these Gameplay Effects apply meaningful cooldowns and resource costs.

## Sub-Tasks

- [ ] Review the constructor of `UGA_BasicAttack` and its Blueprint defaults (if it's a Blueprintable class) to see how `CooldownGameplayEffectClass` and `CostGameplayEffectClass` are set.
- [ ] If they are not set, or set to `nullptr`, identify or create appropriate Gameplay Effect Blueprints for cooldown and cost.
    - Cooldown GE: Should have a duration (e.g., 5 seconds) and apply a tag like `Ability.OnCooldown.BasicAttack`.
    - Cost GE: Should have an instant duration, modify an attribute (e.g., `-10 Mana`), and ensure the attribute is correctly defined in an `URGMAttributeSet`.
- [ ] Assign these GE classes to `CooldownGameplayEffectClass` and `CostGameplayEffectClass` in `UGA_BasicAttack` (either in C++ constructor using `ConstructorHelpers::FClassFinder` or directly in the Blueprint editor if it's a BP-based ability).
- [ ] Verify that `CanApplyGE()` within the Cost GE is correctly configured (e.g., checks if the character has enough resources). The ability's `CanActivateAbility` should also check this via `DoesAbilitySatisfyCost`.
- [ ] Test `UGA_BasicAttack` in-game to confirm:
    - The ability goes on cooldown for the specified duration.
    - The correct resource cost is deducted from the character's attributes.
    - The ability cannot be used if the character cannot afford the cost or if it's on cooldown.

## Files to Examine/Modify

- `Source/RaidGame/Private/Abilities/GA_BasicAttack.cpp`
- `Source/RaidGame/Public/Abilities/GA_BasicAttack.h`
- The Blueprint asset for `UGA_BasicAttack` if it's primarily Blueprint-configured.
- Gameplay Effect Blueprints/C++ classes intended for use as cooldowns and costs (e.g., `GE_DefaultCooldown`, `GE_ManaCost`).
- Attribute Set class (`URGMAttributeSet`) to ensure cost attributes exist.
