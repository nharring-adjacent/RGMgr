#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaiderCommandsWidget.generated.h"

class UButton; // Forward declaration

/**
 * C++ base class for the WBP_RaiderCommands UMG Widget.
 * This widget will display contextual commands for a selected raider.
 */
UCLASS()
class RAIDINGGUILDMANAGER_API URaiderCommandsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called when the widget is constructed.
	// virtual void NativeConstruct() override; // Useful for initial setup if needed

	// These buttons are intended to be bound to UMG Button widgets created in the
	// WBP_RaiderCommands Blueprint, which should have this class as its parent.
	// The names of the UMG buttons must match these variable names exactly.

	UPROPERTY(meta = (BindWidget))
	UButton* TauntButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AttackButton;

protected:
	virtual void NativeConstruct() override; // Standard UMG initialization function

	UFUNCTION()
	void OnTauntButtonClicked();

	UFUNCTION()
	void OnAttackButtonClicked();
};
