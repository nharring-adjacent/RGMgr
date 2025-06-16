# GameMode - Hardcoded Spawn Locations in RaidArenaGameMode

## Justification

Using hardcoded spawn locations (e.g., specific FVector coordinates) in `ARaidArenaGameMode` makes it inflexible and difficult to adjust spawn points without recompiling code. Level designers should be able to define and modify spawn locations using editor-placed objects (like `APlayerStart` or custom spawn point actors). Hardcoding also makes it difficult to support multiple arenas or dynamic spawn configurations. The cost of not fixing this is increased iteration time for level design, reduced map flexibility, and a higher chance of errors if spawn points need to be outside the coded values.

## Problem Description

The `ARaidArenaGameMode::FindPlayerStart_Implementation` or other spawning logic within the GameMode might contain hardcoded `FVector` or `FRotator` values for player and/or AI spawn points.

```cpp
// Hypothetical snippet from ARaidArenaGameMode.cpp
AActor* ARaidArenaGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
    // Example: Ignores PlayerStart actors and returns a hardcoded location
    // This is bad practice for flexibility.
    FVector HardcodedSpawnLocation = FVector(100.0f, 200.0f, 50.0f);
    FRotator HardcodedSpawnRotation = FRotator(0.0f, 90.0f, 0.0f);

    // This might be for player 1, another for player 2, etc.
    // Or for specific teams.
    if (Player && Player->IsA<APlayerController>()) // Or check player index
    {
        // Logic to select one of several hardcoded points
    }

    // This completely bypasses the engine's PlayerStart system if not falling back to Super.
    // return GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), HardcodedSpawnLocation, HardcodedSpawnRotation);
    // More likely, it would try to find a PlayerStart but then override its location,
    // or it constructs a transform to return. For non-player AI, it might directly spawn them at hardcoded points.

    // For AI spawning (e.g., in HandleMatchHasStarted or a custom spawn function):
    // GetWorld()->SpawnActor<AMyAICharacter>(AICharacterClass, FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
    // GetWorld()->SpawnActor<AMyAICharacter>(AICharacterClass, FVector(1000.f, 0.f, 100.f), FRotator::ZeroRotator);

    return Super::FindPlayerStart_Implementation(Player, IncomingName); // Hopefully it calls super if it doesn't find its own.
}

void ARaidArenaGameMode::SpawnRaiders() // Custom function to spawn AI
{
    TArray<FVector> RaiderSpawnPoints;
    RaiderSpawnPoints.Add(FVector(500.f, -200.f, 50.f));
    RaiderSpawnPoints.Add(FVector(500.f, 200.f, 50.f));
    // ... more hardcoded points

    for (const FVector& SpawnPoint : RaiderSpawnPoints)
    {
        // GetWorld()->SpawnActor<ARaiderCharacter>(RaiderClass, SpawnPoint, FRotator::ZeroRotator);
    }
}
```

## Agent Task

Refactor `ARaidArenaGameMode` to use editor-placed spawn point actors (e.g., `APlayerStart` for players, or a custom `ASpawnPoint` actor for AI/teams) instead of hardcoded locations. The GameMode should query for these actors based on tags or other properties.

## Sub-Tasks

- [ ] Review `ARaidArenaGameMode` for any hardcoded `FVector` or `FRotator` values used for spawning players or AI.
- [ ] For player spawning:
    - [ ] Ensure `FindPlayerStart_Implementation` correctly utilizes `APlayerStart` actors placed in the level.
    - [ ] If different players need different spawn points, use `APlayerStart`'s `PlayerStartTag` property and pass the appropriate tag in `IncomingName` or select based on team/player index.
- [ ] For AI spawning (e.g., Raiders):
    - [ ] Create a new C++ or Blueprint actor class (e.g., `ARaidSpawnPoint`) if `APlayerStart` is not suitable (e.g., needs team association or spawn order).
    - [ ] Add properties to `ARaidSpawnPoint` as needed (e.g., `TeamID`, `SpawnType`, `IsEnabled`).
    - [ ] Modify the AI spawning logic in `ARaidArenaGameMode` (e.g., `SpawnRaiders`) to:
        - [ ] Use `UGameplayStatics::GetAllActorsOfClass` (or `TActorIterator`) to find all `ARaidSpawnPoint` (or chosen actor type) instances in the level.
        - [ ] Filter these spawn points based on tags, properties (e.g., `TeamID`), or other criteria.
        - [ ] Use the transforms of these actors for spawning AI.
- [ ] Instruct level designers to place and configure these spawn point actors in arena maps.
- [ ] Remove the hardcoded spawn location values from the C++ code.
- [ ] Test spawning in various maps to ensure players and AI spawn at the locations defined by the placed actors.

## Files to Examine/Modify

- `Source/RaidGame/Private/GameModes/RaidArenaGameMode.cpp`
- `Source/RaidGame/Public/GameModes/RaidArenaGameMode.h`
- Potentially create `Source/RaidGame/Public/World/RaidSpawnPoint.h` and `.cpp`.
- Level files (umap) will need to be updated by a human level designer to place the new spawn points. The agent should note this.
