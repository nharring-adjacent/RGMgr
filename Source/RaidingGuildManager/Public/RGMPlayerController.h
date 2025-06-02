#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RGMPlayerController.generated.h"

// Forward declare ARaiderCharacter to avoid circular dependency if not strictly needed in header
// If we need to operate on ARaiderCharacter members here, include its header.
// For now, a forward declaration for the pointer type is fine.
class ARaiderCharacter;
class URaiderCommandsWidget; // Forward declare the widget class
class ARaidViewCameraActor; // Forward declare the camera actor
template<class T> class TSubclassOf; // Forward declare TSubclassOf

UCLASS()
class RAIDINGGUILDMANAGER_API ARGMPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARGMPlayerController();

protected:
	virtual void BeginPlay() override; // Added BeginPlay for widget creation
	virtual void SetupInputComponent() override;
	void OnLeftMouseClick();

private:
	// Currently selected raider
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Selection", meta = (AllowPrivateAccess = "true"))
	ARaiderCharacter* SelectedRaider;

	// Raider Commands Widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URaiderCommandsWidget> RaiderCommandsWidgetClass;

	UPROPERTY()
	URaiderCommandsWidget* RaiderCommandsWidgetInstance;
};
