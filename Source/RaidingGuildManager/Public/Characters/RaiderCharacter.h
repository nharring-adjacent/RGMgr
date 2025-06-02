// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RGMCharacterBase.h"
#include "Characters/CharacterTypes.h" // Added for ETemporaryDirective
#include "RaiderCharacter.generated.h"
// #include "AI/UtilityAI/UtilityAIComponent.h" // No longer needed here, already in RGMCharacterBase or RaiderCharacter.cpp if specific
#include "Characters/PersonalityStats.h"

class ARaiderAIController; // Forward declaration
class UUtilityAIComponent; // Forward declaration for UtilityAIComponent

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

	// Selection state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	bool bIsSelected;

protected:
	// Materials for selection visual feedback
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	UMaterialInterface* OriginalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection")
	UMaterialInterface* SelectedMaterial;

	// Overrides from RGMCharacterBase
	// virtual void InitializeAttributes() override;
	// virtual void GiveDefaultAbilities() override;

	virtual void BeginPlay() override; // Added BeginPlay for initializing material
	virtual void Tick(float DeltaTime) override;

public:
	// Method to set selection state, might be useful for external control too
	void SetSelected(bool bNewSelectedState);

	// Injects a temporary directive into the AI
	UFUNCTION(BlueprintCallable, Category = "AI")
	void InjectDirective(ETemporaryDirective NewDirective);

private:
	FTimerHandle ClearDirectiveTimerHandle;
	ETemporaryDirective CurrentDirective;
	void ClearDirective();
};
