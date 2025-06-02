#include "UI/RaiderCommandsWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h" // For GetPlayerController
#include "RGMPlayerController.h"   // For ARGMPlayerController
#include "Characters/RaiderCharacter.h" // For ARaiderCharacter and ETemporaryDirective
#include "Characters/CharacterTypes.h"  // For ETemporaryDirective enum

void URaiderCommandsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TauntButton)
	{
		TauntButton->OnClicked.AddDynamic(this, &URaiderCommandsWidget::OnTauntButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("URaiderCommandsWidget: TauntButton is not bound in UMG or is null!"));
	}

	if (AttackButton)
	{
		AttackButton->OnClicked.AddDynamic(this, &URaiderCommandsWidget::OnAttackButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("URaiderCommandsWidget: AttackButton is not bound in UMG or is null!"));
	}
}

void URaiderCommandsWidget::OnTauntButtonClicked()
{
	APlayerController* PC = GetOwningPlayer(); // In UserWidget, GetOwningPlayer() is preferred
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("URaiderCommandsWidget: Could not get Owning Player Controller for Taunt."));
		return;
	}

	ARGMPlayerController* RGM_PC = Cast<ARGMPlayerController>(PC);
	if (RGM_PC && RGM_PC->SelectedRaider)
	{
		RGM_PC->SelectedRaider->InjectDirective(ETemporaryDirective::Taunt);
		UE_LOG(LogTemp, Log, TEXT("Taunt button clicked, directive sent to %s"), *RGM_PC->SelectedRaider->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("URaiderCommandsWidget: RGMPlayerController or SelectedRaider is null for Taunt."));
	}
}

void URaiderCommandsWidget::OnAttackButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("URaiderCommandsWidget: Could not get Owning Player Controller for Attack."));
		return;
	}

	ARGMPlayerController* RGM_PC = Cast<ARGMPlayerController>(PC);
	if (RGM_PC && RGM_PC->SelectedRaider)
	{
		RGM_PC->SelectedRaider->InjectDirective(ETemporaryDirective::Attack);
		UE_LOG(LogTemp, Log, TEXT("Attack button clicked, directive sent to %s"), *RGM_PC->SelectedRaider->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("URaiderCommandsWidget: RGMPlayerController or SelectedRaider is null for Attack."));
	}
}
