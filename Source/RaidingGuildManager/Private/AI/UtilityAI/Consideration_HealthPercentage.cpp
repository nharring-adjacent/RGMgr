// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Consideration_HealthPercentage.h"
#include "Characters/RGMCharacterBase.h"
#include "Abilities/RGMAttributeSet.h"
#include "GameFramework/Controller.h" // For AController
#include "GameFramework/Pawn.h"       // For APawn

float UConsideration_HealthPercentage::ScoreConsideration(AController* Controller, APawn* Pawn)
{
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UConsideration_HealthPercentage: Pawn is null."));
		return 0.0f;
	}

	ARGMCharacterBase* Character = Cast<ARGMCharacterBase>(Pawn);
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("UConsideration_HealthPercentage: Pawn %s is not an ARGMCharacterBase."), *Pawn->GetName());
		return 0.0f;
	}

	const URGMAttributeSet* AttributeSet = Character->GetAttributeSet();
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("UConsideration_HealthPercentage: AttributeSet is null for Pawn %s."), *Pawn->GetName());
		return 0.0f;
	}

	float Health = AttributeSet->GetHealth();
	float MaxHealth = AttributeSet->GetMaxHealth();

	if (MaxHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("UConsideration_HealthPercentage: MaxHealth is 0 or less for Pawn %s."), *Pawn->GetName());
		return 0.0f; // Avoid division by zero
	}

	float HealthPercentage = Health / MaxHealth;
	// UE_LOG(LogTemp, Verbose, TEXT("UConsideration_HealthPercentage: Pawn %s, Health: %f, MaxHealth: %f, Score: %f"), *Pawn->GetName(), Health, MaxHealth, HealthPercentage);
	return HealthPercentage;
}
