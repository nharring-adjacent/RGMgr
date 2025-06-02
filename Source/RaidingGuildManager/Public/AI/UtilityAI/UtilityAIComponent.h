// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UtilityAIAction.h" // Required for TSubclassOf<UUtilityAIAction>
#include "UtilityAIComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RAIDINGGUILDMANAGER_API UUtilityAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUtilityAIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	TArray<TSubclassOf<UUtilityAIAction>> AvailableActions;

	UFUNCTION(BlueprintCallable, Category = "Utility AI")
	UUtilityAIAction* SelectBestAction(AController* Controller, APawn* Pawn);
};
