// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/RaiderCharacter.h"
#include "AI/UtilityAI/UtilityAIComponent.h" // Added for UtilityAIComponent
#include "Abilities/GE_DefaultAttributes.h" // Required for setting DefaultAttributeEffect
#include "Abilities/GA_BasicAttack.h" // Required for adding to DefaultAbilities
#include "AI/RaiderAIController.h" // Required for setting AIControllerClass
#include "AIController.h" // Required for GetController() in Tick
#include "AI/UtilityAI/UtilityAIAction.h" // Required for UUtilityAIAction in Tick
#include "AI/UtilityAI/AIAction_Log.h"     // Added for test action
#include "AI/UtilityAI/AIAction_Idle.h"    // Added for test action


ARaiderCharacter::ARaiderCharacter()
{
	UtilityAIComponent = CreateDefaultSubobject<UUtilityAIComponent>(TEXT("UtilityAIComponent"));
	if (UtilityAIComponent)
	{
		UtilityAIComponent->AvailableActions.Add(UAIAction_Log::StaticClass());
		UtilityAIComponent->AvailableActions.Add(UAIAction_Idle::StaticClass());
	}

	// Set default character class
	CharacterClass = ECharacterClass::None;

	// PersonalityStats will be initialized by its default constructor
	// No explicit initialization needed here for PersonalityStats unless you want to override defaults

	// Set specific defaults for RaiderCharacter if needed
	// For example, different mesh, different base speed, etc.

	// Note: AbilitySystemComponent and AttributeSet are created in the base class (ARGMCharacterBase)
	DefaultAttributeEffect = UGE_DefaultAttributes::StaticClass();
	DefaultAbilities.Add(UGA_BasicAttack::StaticClass());

	// Set the AI Controller class
	AIControllerClass = ARaiderAIController::StaticClass();
}

// Example of overriding InitializeAttributes (Uncomment and implement if needed)
// void ARaiderCharacter::InitializeAttributes()
// {
// 	Super::InitializeAttributes();
//
// 	// Apply a GameplayEffect to set default Raider attributes if different from base
//	// Or directly set them if not using GE:
//	// if (AttributeSet)
//	// {
//	//		AttributeSet->SetHealth(120.f);
//	//		AttributeSet->SetMaxHealth(120.f);
//	// }
// }

// Example of overriding GiveDefaultAbilities (Uncomment and implement if needed)
// void ARaiderCharacter::GiveDefaultAbilities()
// {
// 	Super::GiveDefaultAbilities();
//
// 	// if (AbilitySystemComponent && HasAuthority())
// 	// {
// 	//		// Grant Raider specific abilities
//	//		// AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(URaiderAbility1::StaticClass(), 1, INDEX_NONE, this));
// 	// }
// }

void ARaiderCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Basic check to see if we have a controller and the component
	AController* MyController = GetController();
	if (MyController && UtilityAIComponent)
	{
		UUtilityAIAction* SelectedAction = UtilityAIComponent->SelectBestAction(MyController, this);
		if (SelectedAction)
		{
			// UE_LOG(LogTemp, Warning, TEXT("ARaiderCharacter %s selected action: %s"), *GetName(), *SelectedAction->ActionName);
			// For now, let's also try to execute it if it's valid
			SelectedAction->Execute(MyController, this);
		}
	}
}
