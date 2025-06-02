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
#include "Components/SkeletalMeshComponent.h" // Required for GetMesh()
#include "Materials/MaterialInterface.h" // Required for UMaterialInterface
#include "TimerManager.h" // Required for FTimerManager and SetTimer
#include "AI/UtilityAI/UtilityAIComponent.h" // Ensure UUtilityAIComponent is fully included for its members


ARaiderCharacter::ARaiderCharacter()
{
	bIsSelected = false; // Default to not selected
	OriginalMaterial = nullptr;
	SelectedMaterial = nullptr; // User should set this in Blueprint or C++
	CurrentDirective = ETemporaryDirective::None; // Initialize directive

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

void ARaiderCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Store the original material
	if (GetMesh() && GetMesh()->GetMaterial(0))
	{
		OriginalMaterial = GetMesh()->GetMaterial(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARaiderCharacter %s: Could not get original material from mesh."), *GetName());
	}

	// For demonstration, let's try to load a known engine material if SelectedMaterial is not set.
	// This is a fallback and ideally SelectedMaterial should be set in the editor.
	if (!SelectedMaterial)
	{
		SelectedMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineMaterials/WidgetVertexColorMaterial.WidgetVertexColorMaterial")); // Example material
		if (SelectedMaterial)
		{
			UE_LOG(LogTemp, Log, TEXT("ARaiderCharacter %s: Using fallback SelectedMaterial: %s"), *GetName(), *SelectedMaterial->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ARaiderCharacter %s: Fallback SelectedMaterial could not be loaded. Selection might not be visible."), *GetName());
		}
	}
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

	// Handle selection material swapping
	if (GetMesh())
	{
		UMaterialInterface* CurrentMaterial = GetMesh()->GetMaterial(0);
		if (bIsSelected)
		{
			if (SelectedMaterial && CurrentMaterial != SelectedMaterial)
			{
				GetMesh()->SetMaterial(0, SelectedMaterial);
			}
		}
		else
		{
			if (OriginalMaterial && CurrentMaterial != OriginalMaterial)
			{
				GetMesh()->SetMaterial(0, OriginalMaterial);
			}
		}
	}


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

void ARaiderCharacter::SetSelected(bool bNewSelectedState)
{
	bIsSelected = bNewSelectedState;
}

void ARaiderCharacter::InjectDirective(ETemporaryDirective NewDirective)
{
	CurrentDirective = NewDirective;
	GetWorldTimerManager().ClearTimer(ClearDirectiveTimerHandle); // Clear any existing timer

	UE_LOG(LogTemp, Log, TEXT("ARaiderCharacter %s: Injecting directive: %s"), *GetName(), *UEnum::GetValueAsString(NewDirective));

	if (UtilityAIComponent)
	{
		// --- AI Prioritization Logic Placeholder ---
		// This is where you would integrate with UUtilityAIComponent to force/influence a decision.
		// Since UUtilityAIComponent doesn't have a ForceDecision method yet, we log a placeholder.
		// A real implementation might involve:
		// 1. Adding a method like `ForceAction(FName ActionName, float Duration)` to UUtilityAIComponent.
		// 2. Or, adding properties to UUtilityAIComponent that considerations can check (e.g. `CurrentForcedDirective`).
		// 3. Or, directly manipulating scores of certain actions temporarily.

		FName ActionToForce = NAME_None;
		if (NewDirective == ETemporaryDirective::Taunt)
		{
			ActionToForce = TEXT("Taunt"); // Assuming an action named "Taunt" exists in AvailableActions
		}
		else if (NewDirective == ETemporaryDirective::Attack)
		{
			ActionToForce = TEXT("Attack"); // Assuming an action named "Attack" exists in AvailableActions
		}

		if (ActionToForce != NAME_None)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARaiderCharacter %s: Placeholder - Would force AI action '%s' if UUtilityAIComponent supported it."), *GetName(), *ActionToForce.ToString());
			// Example of what could be: UtilityAIComponent->ForceDecision(ActionToForce, 5.0f);
		}
		else if (NewDirective != ETemporaryDirective::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARaiderCharacter %s: Unknown directive %s. No AI action forced."), *GetName(), *UEnum::GetValueAsString(NewDirective));
		}
		// --- End AI Prioritization Logic Placeholder ---
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARaiderCharacter %s: UtilityAIComponent is null. Cannot inject directive."), *GetName());
	}

	if (NewDirective != ETemporaryDirective::None)
	{
		// Set a timer to clear the directive after 5 seconds
		GetWorldTimerManager().SetTimer(ClearDirectiveTimerHandle, this, &ARaiderCharacter::ClearDirective, 5.0f, false);
	}
}

void ARaiderCharacter::ClearDirective()
{
	UE_LOG(LogTemp, Log, TEXT("ARaiderCharacter %s: Clearing directive. Previous: %s"), *GetName(), *UEnum::GetValueAsString(CurrentDirective));
	CurrentDirective = ETemporaryDirective::None;

	if (UtilityAIComponent)
	{
		// --- AI Prioritization Clearing Logic Placeholder ---
		// If a ForceDecision or override was applied, this is where you'd clear it.
		// Example: UtilityAIComponent->ClearForcedDecision();
		UE_LOG(LogTemp, Warning, TEXT("ARaiderCharacter %s: Placeholder - Would clear forced AI action if UUtilityAIComponent supported it."), *GetName());
		// --- End AI Prioritization Clearing Logic Placeholder ---
	}

	// Timer handle is automatically cleared by TimerManager when it executes a non-looping timer.
	// ClearDirectiveTimerHandle.Invalidate(); // Not strictly necessary for non-looping, but good for clarity if re-used.
}
