#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateStaminaModifier.generated.h"

UCLASS()
class RAIDINGGUILDMANAGER_API UBTService_UpdateStaminaModifier : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateStaminaModifier();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
