// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BossCharacter.h"
#include "Abilities/GE_DefaultAttributes.h" // Required for setting DefaultAttributeEffect
#include "Abilities/GA_BasicAttack.h" // Required for adding to DefaultAbilities
#include "AI/BossAIController.h" // Required for setting AIControllerClass

ABossCharacter::ABossCharacter()
{
	// Set specific defaults for BossCharacter if needed
	// For example, larger mesh, unique boss-specific components, etc.

	// Note: AbilitySystemComponent and AttributeSet are created in the base class (ARGMCharacterBase)
	DefaultAttributeEffect = UGE_DefaultAttributes::StaticClass();
	DefaultAbilities.Add(UGA_BasicAttack::StaticClass());

	// Set the AI Controller class
	AIControllerClass = ABossAIController::StaticClass();
}

// Example of overriding InitializeAttributes (Uncomment and implement if needed)
// void ABossCharacter::InitializeAttributes()
// {
// 	Super::InitializeAttributes();
//
// 	// Apply a GameplayEffect to set default Boss attributes
//	// if (AttributeSet)
//	// {
//	//		AttributeSet->SetHealth(1000.f);
//	//		AttributeSet->SetMaxHealth(1000.f);
//	//		AttributeSet->SetMana(500.f);
//	//		AttributeSet->SetMaxMana(500.f);
//	// }
// }

// Example of overriding GiveDefaultAbilities (Uncomment and implement if needed)
// void ABossCharacter::GiveDefaultAbilities()
// {
// 	Super::GiveDefaultAbilities();
//
// 	// if (AbilitySystemComponent && HasAuthority())
// 	// {
// 	//		// Grant Boss specific abilities
//	//		// AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UBossAbility1::StaticClass(), 1, INDEX_NONE, this));
// 	// }
// }
