// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BossCharacter.h"
#include "Characters/RaiderCharacter.h" // Added for threat system
#include "Abilities/AttributeSet/RGMAttributeSet.h" // For accessing Health, MaxHealth if needed directly
#include "AbilitySystemComponent.h" // For AbilitySystemComponent
#include "Abilities/GE_DefaultAttributes.h" // Required for setting DefaultAttributeEffect
#include "Abilities/GA_BasicAttack.h" // Required for adding to DefaultAbilities
#include "AI/BossAIController.h" // Required for setting AIControllerClass
#include "GameFramework/Controller.h" // For AController in TakeDamage
#include "DrawDebugHelpers.h" // Added for debug drawing

ABossCharacter::ABossCharacter()
{
	// Note: AbilitySystemComponent and AttributeSet are created in the base class (ARGMCharacterBase)
	// Default attributes are applied via DefaultAttributeEffect in ARGMCharacterBase::InitializeAttributes
	DefaultAttributeEffect = UGE_DefaultAttributes::StaticClass(); // This is likely where MaxHealth is set
	DefaultAbilities.Add(UGA_BasicAttack::StaticClass());

	// Set the AI Controller class
	AIControllerClass = ABossAIController::StaticClass();

	// Health and MaxHealth are assumed to be initialized by the AttributeSet via DefaultAttributeEffect
	// If direct initialization was needed here (not typical with GAS):
	// if (AttributeSet)
	// {
	// AttributeSet->SetMaxHealth(1000.0f);
	// AttributeSet->SetHealth(GetMaxHealth());
	// }
}

void ABossCharacter::ReceiveThreat(ARaiderCharacter* SourceRaider, float Amount)
{
	if (!SourceRaider)
	{
		return;
	}

	float& CurrentThreat = ThreatMap.FindOrAdd(SourceRaider);
	CurrentThreat += Amount;
	CurrentThreat = FMath::Max(0.0f, CurrentThreat); // Ensure threat doesn't go below 0

	// UE_LOG(LogTemp, Log, TEXT("Threat update: %s now has %.2f threat on %s."), *SourceRaider->GetName(), CurrentThreat, *GetName());
	// Using FindChecked as we've just added/updated it.
	UE_LOG(LogTemp, Log, TEXT("Boss Threat Update: Raider %s now has %.2f threat on %s."), *SourceRaider->GetName(), ThreatMap.FindChecked(SourceRaider), *GetName());
}

ARaiderCharacter* ABossCharacter::GetHighestThreatTarget() const
{
	ARaiderCharacter* HighestThreatTarget = nullptr;
	float MaxThreat = -1.0f; // Start with a value lower than any possible threat

	for (const auto& Pair : ThreatMap)
	{
		if (Pair.Key && Pair.Value > MaxThreat)
		{
			MaxThreat = Pair.Value;
			HighestThreatTarget = Pair.Key;
		}
	}
	return HighestThreatTarget;
}

float ABossCharacter::GetThreatForRaider(ARaiderCharacter* Raider) const
{
	if (!Raider)
	{
		return 0.0f;
	}
	const float* FoundThreat = ThreatMap.Find(Raider);
	return FoundThreat ? *FoundThreat : 0.0f;
}

void ABossCharacter::AttackTarget(ARaiderCharacter* Target)
{
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s tried to attack a null target."), *GetName());
		return;
	}
	// For now, just log. Later this could trigger an ability or animation.
	UE_LOG(LogTemp, Warning, TEXT("%s attacks %s! (Placeholder)"), *GetName(), *Target->GetName());
}

float ABossCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		// Health is managed by the AttributeSet, updated by Super::TakeDamage.
		// We can log it here or react to it.
		if (AttributeSet)
		{
			UE_LOG(LogTemp, Log, TEXT("%s took %.2f damage. Current Health: %.2f/%.2f"),
				*GetName(), ActualDamage, AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());

			if (AttributeSet->GetHealth() <= 0.0f)
			{
				// HandleDeath is virtual in RGMCharacterBase, so it can be overridden for boss-specific death behavior
				HandleDeath();
			}
		}
	}
	return ActualDamage;
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ARaiderCharacter* CurrentTarget = GetHighestThreatTarget();
	if (CurrentTarget)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), CurrentTarget->GetActorLocation(), FColor::Red, false, -1, 0, 5.f);
		DrawDebugString(GetWorld(), GetActorLocation() + FVector(0,0,100.f), FString::Printf(TEXT("Boss Targeting: %s (Threat: %.2f)"), *CurrentTarget->GetName(), GetThreatForRaider(CurrentTarget)), nullptr, FColor::Red, 0.0f, false, 0.8f);
	}

	// Log all threat values (can be spammy, set to Verbose)
	// for (const auto& Pair : ThreatMap)
	// {
	// 	if (Pair.Key) // Check if the Raider pointer is valid
	// 	{
	// 		UE_LOG(LogTemp, Verbose, TEXT("Boss Threat Map: %s -> %.2f"), *Pair.Key->GetName(), Pair.Value);
	// 	}
	// }
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
