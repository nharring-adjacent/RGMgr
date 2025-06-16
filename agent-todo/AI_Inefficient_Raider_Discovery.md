# AI - Inefficient Raider Discovery

## Justification

Repeatedly using `TActorIterator` to find a specific Raider AI Controller by ID within `ARaidLeaderAIController::FindRaiderControllerByID` can be inefficient, especially if this function is called frequently or if the number of Raider AI Controllers in the world is large. Each iteration involves traversing a list of actors, which can lead to performance degradation. The cost of not fixing this is potential performance bottlenecks in AI-heavy scenarios, leading to lower frame rates or sluggish AI behavior.

## Problem Description

The function `ARaidLeaderAIController::FindRaiderControllerByID` likely iterates over all `ARaiderAIController` instances in the world every time it's called. If this function is part of a frequent AI logic tick or event, this repeated iteration is suboptimal.

```cpp
// Hypothetical snippet from ARaidLeaderAIController.cpp
ARaiderAIController* ARaidLeaderAIController::FindRaiderControllerByID(int32 RaiderID)
{
    for (TActorIterator<ARaiderAIController> It(GetWorld()); It; ++It)
    {
        ARaiderAIController* RaiderController = *It;
        if (RaiderController && RaiderController->GetRaiderID() == RaiderID) // Assuming GetRaiderID() exists
        {
            return RaiderController;
        }
    }
    return nullptr;
}
```

## Agent Task

Refactor `ARaidLeaderAIController::FindRaiderControllerByID` to avoid repeated `TActorIterator` usage. Implement a more efficient way to look up Raider AI Controllers, such as caching them in a map on initialization or when they spawn/despawn.

## Sub-Tasks

- [ ] Analyze `ARaidLeaderAIController` to understand when and how Raider AI Controllers are spawned and potentially destroyed.
- [ ] Add a `TMap<int32, ARaiderAIController*>` to `ARaidLeaderAIController` to store references to Raider AI Controllers, keyed by their ID.
- [ ] Populate this map when Raider AI Controllers are initialized (e.g., in `BeginPlay` or a dedicated registration function).
- [ ] Implement logic to remove Raider AI Controllers from the map if they are destroyed or despawned.
- [ ] Modify `FindRaiderControllerByID` to use the map for lookups.
- [ ] Ensure the solution handles potential edge cases, such as a Raider ID not being found or a controller becoming invalid.
- [ ] Review calls to `FindRaiderControllerByID` to ensure they correctly handle the possibility of a null return if a controller is not found or is pending spawn.

## Files to Examine/Modify

- `Source/RaidGame/Private/AI/ARaidLeaderAIController.cpp`
- `Source/RaidGame/Public/AI/ARaidLeaderAIController.h`
- `Source/RaidGame/Private/AI/ARaiderAIController.cpp` (if `GetRaiderID` or spawn logic needs changes)
- `Source/RaidGame/Public/AI/ARaiderAIController.h` (if `GetRaiderID` or spawn logic needs changes)
