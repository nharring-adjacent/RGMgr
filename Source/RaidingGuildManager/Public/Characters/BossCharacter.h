// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RGMCharacterBase.h"
#include "Characters/RaiderCharacter.h" // Added for ARaiderCharacter
#include "BossCharacter.generated.h"

class ABossAIController; // Forward declaration
class ARaiderCharacter; // Forward declaration for TMap value

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ABossCharacter : public ARGMCharacterBase
{
	GENERATED_BODY()

public:
	ABossCharacter();

	//~ Begin Threat Mechanics
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Threat")
	TMap<ARaiderCharacter*, float> ThreatMap;

	UFUNCTION(BlueprintCallable, Category = "AI|Threat")
	void ReceiveThreat(ARaiderCharacter* SourceRaider, float Amount);

	UFUNCTION(BlueprintCallable, Category = "AI|Threat")
	ARaiderCharacter* GetHighestThreatTarget() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Threat")
	float GetThreatForRaider(ARaiderCharacter* Raider) const;
	//~ End Threat Mechanics

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTarget(ARaiderCharacter* Target);

	//~ Begin AActor Interface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//~ End AActor Interface

protected:
	// Overrides from RGMCharacterBase
	// virtual void InitializeAttributes() override; // Handled by AttributeSet in RGMCharacterBase
	// virtual void GiveDefaultAbilities() override;

	virtual void Tick(float DeltaTime) override;
};
