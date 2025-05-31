#include "AI/BTService_UpdateStaminaModifier.h"
#include "AI/RaiderAIController.h"
#include "GameModes/RaidArenaGameMode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_UpdateStaminaModifier::UBTService_UpdateStaminaModifier()
{
	NodeName = "Update Stamina Modifier";
	// Set interval (e.g., every 0.5 seconds)
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_UpdateStaminaModifier::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARaiderAIController* AIController = Cast<ARaiderAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
	{
		return;
	}

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(AIController->GetWorld());
	ARaidArenaGameMode* ArenaGameMode = Cast<ARaidArenaGameMode>(GameMode);

	if (ArenaGameMode)
	{
		float StaminaModifier = ArenaGameMode->GetStaminaPerformanceModifier();
		BlackboardComp->SetValueAsFloat(ARaiderAIController::StaminaModifierKeyName, StaminaModifier);
		// UE_LOG(LogTemp, Verbose, TEXT("Stamina Modifier Updated in BB: %f"), StaminaModifier);
	}
	else
	{
		// If not in arena game mode, or if cast fails, set a default value
		BlackboardComp->SetValueAsFloat(ARaiderAIController::StaminaModifierKeyName, 1.0f);
		// UE_LOG(LogTemp, Warning, TEXT("ArenaGameMode not found, Stamina Modifier set to 1.0"));
	}
}
