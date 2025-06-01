// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/RaiderCharacter.h"
#include "AI/RaiderAIController.h"    // Required for setting AIControllerClass
#include "Abilities/GA_BasicAttack.h" // Required for adding to DefaultAbilities
#include "Abilities/GE_DefaultAttributes.h" // Required for setting DefaultAttributeEffect

ARaiderCharacter::ARaiderCharacter() {
  // Set default character class
  CharacterClass = ECharacterClass::None;

  // PersonalityStats will be initialized by its default constructor
  // No explicit initialization needed here for PersonalityStats unless you want
  // to override defaults

  // Set specific defaults for RaiderCharacter if needed
  // For example, different mesh, different base speed, etc.

  // Note: AbilitySystemComponent and AttributeSet are created in the base class
  // (ARGMCharacterBase)
  DefaultAttributeEffect = UGE_DefaultAttributes::StaticClass();
  DefaultAbilities.Add(UGA_BasicAttack::StaticClass());

  // Set the AI Controller class
  AIControllerClass = ARaiderAIController::StaticClass();
}

// Example of overriding InitializeAttributes (Uncomment and implement if
// needed) void ARaiderCharacter::InitializeAttributes()
// {
// 	Super::InitializeAttributes();
//
// 	// Apply a GameplayEffect to set default Raider attributes if different
// from base
//	// Or directly set them if not using GE:
//	// if (AttributeSet)
//	// {
//	//		AttributeSet->SetHealth(120.f);
//	//		AttributeSet->SetMaxHealth(120.f);
//	// }
// }

// Example of overriding GiveDefaultAbilities (Uncomment and implement if
// needed) void ARaiderCharacter::GiveDefaultAbilities()
// {
// 	Super::GiveDefaultAbilities();
//
// 	// if (AbilitySystemComponent && HasAuthority())
// 	// {
// 	//		// Grant Raider specific abilities
//	//		//
//AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(URaiderAbility1::StaticClass(),
//1, INDEX_NONE, this));
// 	// }
// }
