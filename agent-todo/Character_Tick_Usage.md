# Character - Review Necessity of Default Tick in ARGMCharacterBase

## Justification

`AActor::PrimaryActorTick.bCanEverTick` defaults to `true` for many actor types, including `ACharacter`. If `ARGMCharacterBase` (and potentially its derived classes) do not actually require per-frame updates in their `Tick()` function, leaving it enabled is a minor waste of resources. The engine will still call `Tick()` even if it's empty, incurring a small overhead for each instance. For a large number of characters, this can add up. The cost of not fixing this is a small but potentially widespread performance drain, making the game slightly less efficient than it could be.

## Problem Description

The `ARGMCharacterBase` class might have `PrimaryActorTick.bCanEverTick = true` set in its constructor (or rely on the parent class default) but may not implement any crucial logic in its `Tick(float DeltaTime)` method.

```cpp
// In ARGMCharacterBase.cpp or .h
ARGMCharacterBase::ARGMCharacterBase()
{
    // If bCanEverTick is true (default for ACharacter)
    // but Tick() is empty or does non-essential work:
    PrimaryActorTick.bCanEverTick = true;
}

void ARGMCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // If there's no specific logic here, or only debug logic,
    // then ticking might be unnecessary.
}
```

## Agent Task

Review the `Tick()` function of `ARGMCharacterBase` and its derived classes. If ticking is not necessary for core gameplay logic, disable it by default. If certain instances or child classes *do* require ticking, they can enable it specifically.

## Sub-Tasks

- [ ] Examine the `ARGMCharacterBase::Tick()` method.
- [ ] Identify any logic within the `Tick()` method.
- [ ] Determine if this logic is essential for the character's functionality or if it can be event-driven or moved elsewhere (e.g., to a component that explicitly ticks).
- [ ] If the `Tick()` logic is not essential or the method is empty, set `PrimaryActorTick.bCanEverTick = false;` in the `ARGMCharacterBase` constructor.
- [ ] Set `PrimaryActorTick.bStartWithTickEnabled = false;` as well if `bCanEverTick` is true but ticking isn't needed immediately.
- [ ] Identify any child classes of `ARGMCharacterBase`. Review their `Tick()` methods as well.
- [ ] If a child class *does* need to tick, ensure it explicitly sets `PrimaryActorTick.bCanEverTick = true;` (and `bStartWithTickEnabled = true;` if needed) in its own constructor.
- [ ] Test the game to ensure that disabling tick by default does not break any character behaviors. Pay attention to animations, custom movement, or other systems that might implicitly rely on actor tick.

## Files to Examine/Modify

- `Source/RaidGame/Public/Character/RGMCharacterBase.h`
- `Source/RaidGame/Private/Character/RGMCharacterBase.cpp`
- Header and source files of any classes derived from `ARGMCharacterBase`.
