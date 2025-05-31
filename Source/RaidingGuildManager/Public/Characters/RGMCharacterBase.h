// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h" // Required for IAbilitySystemInterface
#include "RGMCharacterBase.generated.h"

class UAbilitySystemComponent;
class URGMAttributeSet; // Forward declare our AttributeSet
class UGameplayEffect; // Forward declare for TSubclassOf

UCLASS(Abstract) // Abstract means this class cannot be spawned directly, only derived classes
class RAIDINGGUILDMANAGER_API ARGMCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARGMCharacterBase();

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AttributeSet
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	URGMAttributeSet* AttributeSet;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Initialize attributes (e.g. default health, mana)
	virtual void InitializeAttributes();
	// Grant default abilities (e.g. basic attack)
	virtual void GiveDefaultAbilities();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	// Called when possession happens, good place to init ASC for AI
	virtual void PossessedBy(AController* NewController) override;
	// Called on the client when replication of the PlayerState occurs
	virtual void OnRep_PlayerState() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when health reaches <= 0
	virtual void HandleDeath();

	// Delegate broadcasted when character dies
	FSimpleMulticastDelegate OnCharacterDied;
};
