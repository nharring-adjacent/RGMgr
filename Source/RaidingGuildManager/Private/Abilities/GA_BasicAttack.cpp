// Copyright Epic Games, Inc. All Rights Reserved.

#include "Abilities/GA_BasicAttack.h"
#include "Abilities/GE_DamageBasic.h" // For setting the default class and for casting
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Characters/RGMCharacterBase.h" // To get target actor for applying effect

UGA_BasicAttack::UGA_BasicAttack()
{
	// Set default for the damage effect class
	DamageGameplayEffectClass = UGE_DamageBasic::StaticClass();
	AttackDamageAmount = 10.0f; // Default damage for this ability

	// Define tags for this ability
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Basic")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Basic"))); // Tags applied to owner while ability is active

	// Cooldown (optional)
	// UGameplayEffect* CooldownEffect = CreateDefaultSubobject<UGameplayEffect>(TEXT("CooldownEffect"));
	// CooldownGameplayEffectClass = CooldownEffect->GetClass(); // This is not how you set cooldown GE

	// Cost (optional)
	// UGameplayEffect* CostEffect = CreateDefaultSubobject<UGameplayEffect>(TEXT("CostEffect"));
	// CostGameplayEffectClass = CostEffect->GetClass(); // This is not how you set cost GE
}

bool UGA_BasicAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// Add any specific checks here, e.g., if the character is alive, has a target, etc.
	// For now, always allow activation if base class allows it.
	return true;
}

void UGA_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// For this basic attack, we'll assume the target is passed via TriggerEventData or obtained through other means (e.g., AI target).
	// As a placeholder, let's try to get a target from event data.
	// A more robust solution would involve GameplayTargetData.
	AActor* TargetActor = nullptr;
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		TargetActor = TriggerEventData->TargetData.Get(0)->GetHitResult()->GetActor(); // Example if target data is from a line trace
		// Or if it's a direct actor reference:
		// TargetActor = Cast<AActor>(TriggerEventData->TargetData.Get(0)->GetActors()[0].Get());
	}

	// If no target from event data, for testing, let's make AI target its current focus or player target self (if no other target system yet)
	// This part is highly dependent on how targeting is implemented.
	// For now, we'll just apply to self if no target is found, which is not typical for an attack.
	// The BTTask will provide the target.

	if (!DamageGameplayEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_BasicAttack: DamageGameplayEffectClass is not set!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Create a spec for the damage effect
	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffectClass, GetAbilityLevel());

	if (DamageEffectSpecHandle.IsValid())
	{
		// Modify the spec's modifier magnitude for health.
		// This assumes the first modifier in GE_DamageBasic affects health.
		// A more robust way is to use SetByCallerMagnitude with a specific tag.

		float FinalDamageAmount = AttackDamageAmount;
		if (TriggerEventData)
		{
			float StaminaModifier = TriggerEventData->EventMagnitude;
			// Ensure modifier is not zero or negative if it's a multiplier
			if (StaminaModifier > 0.0f)
			{
				FinalDamageAmount *= StaminaModifier;
				UE_LOG(LogTemp, Log, TEXT("UGA_BasicAttack: Applying StaminaModifier %f. OriginalDamage: %f, FinalDamage: %f"), StaminaModifier, AttackDamageAmount, FinalDamageAmount);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UGA_BasicAttack: Invalid StaminaModifier %f received. Using original damage."), StaminaModifier);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGA_BasicAttack: TriggerEventData is null. Cannot apply StaminaModifier. Using original damage %f."), AttackDamageAmount);
		}

		if (DamageEffectSpecHandle.Data->Modifiers.IsValidIndex(0))
		{
			DamageEffectSpecHandle.Data->Modifiers[0].SetMagnitude(FinalDamageAmount); // Negative because it's damage
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGA_BasicAttack: DamageGameplayEffectClass has no modifiers to set magnitude!"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}


		// Attempt to get target from the ability system component's current generic target actor
		// This is a common pattern for AI.
		if(!TargetActor)
		{
			// This is a placeholder for a real targeting system.
			// In a real game, TargetActor would be determined by AI, player input, etc.
			// For now, if an AI is using this, the BTTask_AttackTarget should provide the target.
			// If a player is using this, target acquisition logic is needed.
			// As a fallback for testing, let's try to apply to self if no target found by other means.
			// THIS IS NOT IDEAL for an attack ability.
			// TargetActor = ActorInfo->AvatarActor.Get();
			UE_LOG(LogTemp, Display, TEXT("UGA_BasicAttack: No target found, ability will likely fail or do nothing unless BT provides target."));
		}

		// The BTTask_AttackTarget will be responsible for providing the target.
		// For now, we'll proceed if a target is somehow acquired.
		// The ApplyGameplayEffectToTarget expects an AbilitySystemComponent.
		// We need to get the ASC from the TargetActor.

		// The actual application to a target will be handled by the BTTask_AttackTarget
		// which will call this ability and pass target data.
		// For now, we just prepare the spec. The BT task will apply it.
		// If this ability were player-controlled, it would handle target data here.

		// For now, let's assume the ability is activated with a target in mind by the BT.
		// The BT Task will fetch this SpecHandle and apply it.
		// This is a bit of a workaround as abilities usually apply their own effects.
		// A better way is to use an AbilityTask_ApplyGameplayEffectToTarget or similar.

		// Let's try to apply to a target if available from event data (for non-AI use or direct calls with event data)
		if (TargetActor)
		{
			IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor);
			if (ASI)
			{
				UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
				if (TargetASC)
				{
					ApplyGameplayEffectSpecToTarget(Handle, ActorInfo, ActivationInfo, DamageEffectSpecHandle, TargetASC);
					UE_LOG(LogTemp, Display, TEXT("UGA_BasicAttack: Applied damage spec to %s"), *TargetActor->GetName());
				}
			}
		} else {
			// If no target, the BTTask_AttackTarget is expected to handle this.
			// We can store the spec handle if the BT needs to retrieve it.
			// This is not standard. Abilities typically complete their action.
		}

	} else {
		UE_LOG(LogTemp, Warning, TEXT("UGA_BasicAttack: Failed to make outgoing spec for damage effect!"));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false); // false, false means success, don't cancel
}
