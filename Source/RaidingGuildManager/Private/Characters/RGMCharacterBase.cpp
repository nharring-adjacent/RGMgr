// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/RGMCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/RGMAttributeSet.h" // We need the full definition here
#include "GameplayEffect.h" // Required for TSubclassOf<UGameplayEffect>
#include "GameplayAbility.h" // Required for TSubclassOf<UGameplayAbility>
#include "GameFramework/PlayerController.h" // For OnRep_PlayerState

// Sets default values
ARGMCharacterBase::ARGMCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Could be Mixed or Full for more complex scenarios

	// Create the attribute set
	AttributeSet = CreateDefaultSubobject<URGMAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void ARGMCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

UAbilitySystemComponent* ARGMCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARGMCharacterBase::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this); // The source object of this effect is this character
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ARGMCharacterBase::GiveDefaultAbilities()
{
	if (AbilitySystemComponent && HasAuthority()) // HasAuthority() ensures this only runs on the server
	{
		for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			if (StartupAbility)
			{
				AbilitySystemComponent->GiveAbility(
					FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
			}
		}
	}
}

void ARGMCharacterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Server-side initialization of AbilitySystemComponent
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		GiveDefaultAbilities();
    }
}

void ARGMCharacterBase::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // Client-side initialization of AbilitySystemComponent
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		// Note: GiveDefaultAbilities() is typically server-only.
		// Client will get abilities through replication from the server.
		// If you have client-side predicted abilities that need to be granted on spawn, you might do it here.
    }
}

// Called every frame
void ARGMCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARGMCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARGMCharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s has died!"), *GetName());
	OnCharacterDied.Broadcast();

	// Optional: Stop AI, play death animation, detach controller, set lifespan
	// AController* CurrentController = GetController();
	// if (CurrentController)
	// {
	// 	CurrentController->StopMovement();
	// 	// DetachFromControllerPendingDestroy also calls UnPossess
	// }
	// DetachFromControllerPendingDestroy();

	// GetMesh()->PlayAnimation(DeathAnimation, false); // Assuming DeathAnimation is a UAnimSequence*
	// SetLifeSpan(5.0f); // Actor will be destroyed after 5 seconds

	// For now, just disable collision and stop ticking
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	// Stop any active abilities
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
}
