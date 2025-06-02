// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RGMCharacterBase.h"
#include "AI/UtilityAI/UtilityAIComponent.h" // Added for UtilityAIComponent
#include "RaiderCharacter.generated.h"
#include "Characters/CharacterTypes.h"
#include "Characters/PersonalityStats.h"

class ARaiderAIController; // Forward declaration
class UUtilityAIComponent; // Forward declaration for UtilityAIComponent
class ABossCharacter;    // Forward declaration for BossCharacter

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ARaiderCharacter : public ARGMCharacterBase
{
	GENERATED_BODY()

public:
	ARaiderCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UUtilityAIComponent* UtilityAIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Class")
	ECharacterClass CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
	FPersonalityStats PersonalityStats;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//~ Combat Functions
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void DealDamageToTarget(ACharacter* TargetCharacter, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void GenerateThreatForTarget(ABossCharacter* TargetBoss, float ThreatAmount);
	//~ End Combat Functions

protected:
	// Overrides from RGMCharacterBase
	// virtual void InitializeAttributes() override;
	// virtual void GiveDefaultAbilities() override;
};
