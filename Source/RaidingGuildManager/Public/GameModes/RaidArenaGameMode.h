// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Components/RaidPartyStaminaComponent.h"
#include "RaidArenaGameMode.generated.h"

class URaidPartyStaminaComponent;

/**
 * ARaidArenaGameMode
 *
 * This game mode is intended for the "Test_RaidArena" map.
 * It is responsible for spawning the selected raiders and a boss when the match starts.
 *
 * Assignment:
 * This GameMode should be set as the default GameMode for the 'Test_RaidArena' map.
 * This can be done in the Unreal Editor via:
 * 1. Project Settings -> Maps & Modes -> Default GameMode (if this is the only game mode or for general testing).
 * 2. OR, more specifically, by opening the 'Test_RaidArena' map and in World Settings,
 *    setting the 'GameMode Override' to this 'ARaidArenaGameMode'.
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ARaidArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void StartPlay() override;
	// virtual void HandleMatchHasStarted() override; // Alternative to StartPlay

private:
	bool bIsInCombat;
	float OutOfCombatTimer;
	bool bIsInBreakPeriod;

public:
	ARaidArenaGameMode();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartCombat();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EndCombat();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartBreakPeriod();

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void EndBreakPeriod();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stamina")
	float GetStaminaPerformanceModifier() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	URaidPartyStaminaComponent* RaidPartyStaminaComponent;
};
