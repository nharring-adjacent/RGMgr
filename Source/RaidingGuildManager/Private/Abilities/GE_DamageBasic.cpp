// Copyright Epic Games, Inc. All Rights Reserved.

#include "Abilities/GE_DamageBasic.h"
#include "Abilities/RGMAttributeSet.h" // Required for GetHealthAttribute

UGE_DamageBasic::UGE_DamageBasic()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	DamageAmount = -10.0f; // Default damage value, can be overridden by the ability

	FGameplayModifierInfo DamageModifier;
	DamageModifier.Attribute = URGMAttributeSet::GetHealthAttribute();
	DamageModifier.ModifierOp = EGameplayModOp::Additive;
	// The magnitude will be set by the GameplayAbility that creates an instance of this effect
	// However, we can provide a default magnitude source, e.g. a SetByCaller
	// For now, we'll assume the ability directly modifies the DamageAmount on the effect spec or instance
	// Or, more simply, we could set it here if it were fixed, but it's meant to be variable.
	// A common practice is to use a SetByCaller magnitude, and then the ability sets that value.
	// For simplicity in this step, we'll rely on the ability creating an effect spec and setting DamageAmount on it,
	// which then needs to be wired up to the modifier magnitude.

	// A more robust way for variable damage:
	// 1. Add a FGameplayEffectAttributeCaptureDefinition for a tag, or use SetByCaller.
	//    FScalableFloat DamageMagnitude = FScalableFloat(DamageAmount); // This won't work directly as DamageAmount is not static
	//    DamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageMagnitude);
	// For now, let's keep it simple and assume the ability will create a dynamic effect spec.
	// The simplest way to make DamageAmount effective without SetByCaller is to make the ability create
	// an effect spec, then get the spec, cast its UGameplayEffect* to UGE_DamageBasic* and set DamageAmount,
	// then add the modifier with that amount. This is a bit clunky.
	// The alternative is using SetByCaller with a data tag.

	// Let's use SetByCaller for DamageAmount.
	// The ability will need to use `WithSourceTags` and `SetSetByCallerMagnitude`
	DamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.0f)); // Placeholder, will be overridden by SetByCaller

	// To make this work with SetByCaller, we would typically add a tag to the effect and then
	// in the ability do something like:
	// FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffectClass);
	// SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), DamageToApply);
	// And then the modifier would need to be configured to use that tag.
	// For now, the GA_BasicAttack will manually create the modifier with the damage amount.
	// This is less ideal than SetByCaller but simpler for this step.
	// Modifiers.Add(DamageModifier); // We'll add it dynamically in the ability for now.

	// Re-simplifying: The ability will create an instance of this effect,
	// and we expect the ability to *configure* the created spec's modifier directly.
	// So, the modifier added here should be a template.
	// The most common way is to have the GE define the attribute and op, and the ability provides magnitude.
	FGameplayModifierInfo HealthMod;
	HealthMod.Attribute = URGMAttributeSet::GetHealthAttribute();
	HealthMod.ModifierOp = EGameplayModOp::Additive;
	HealthMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(0.f); // Will be overridden by instigator
	Modifiers.Add(HealthMod);
}
