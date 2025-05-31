// Copyright Epic Games, Inc. All Rights Reserved.

#include "Abilities/GE_DefaultAttributes.h"
#include "Abilities/RGMAttributeSet.h" // Required to get attribute accessors

UGE_DefaultAttributes::UGE_DefaultAttributes()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Add Modifiers for Health and MaxHealth
	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = URGMAttributeSet::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Override; // Or Additive if you want to add to a base value from elsewhere
	HealthModifier.ModifierMagnitude = FScalableFloat(100.0f); // Default Health Value
	Modifiers.Add(HealthModifier);

	FGameplayModifierInfo MaxHealthModifier;
	MaxHealthModifier.Attribute = URGMAttributeSet::GetMaxHealthAttribute();
	MaxHealthModifier.ModifierOp = EGameplayModOp::Override;
	MaxHealthModifier.ModifierMagnitude = FScalableFloat(100.0f); // Default MaxHealth Value
	Modifiers.Add(MaxHealthModifier);

	// Add Modifiers for Mana and MaxMana
	FGameplayModifierInfo ManaModifier;
	ManaModifier.Attribute = URGMAttributeSet::GetManaAttribute();
	ManaModifier.ModifierOp = EGameplayModOp::Override;
	ManaModifier.ModifierMagnitude = FScalableFloat(50.0f); // Default Mana Value
	Modifiers.Add(ManaModifier);

	FGameplayModifierInfo MaxManaModifier;
	MaxManaModifier.Attribute = URGMAttributeSet::GetMaxManaAttribute();
	MaxManaModifier.ModifierOp = EGameplayModOp::Override;
	MaxManaModifier.ModifierMagnitude = FScalableFloat(50.0f); // Default MaxMana Value
	Modifiers.Add(MaxManaModifier);
}
