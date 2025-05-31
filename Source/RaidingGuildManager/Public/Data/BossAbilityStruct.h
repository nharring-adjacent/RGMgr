#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h" // For ability tags
#include "BossAbilityStruct.generated.h"

USTRUCT(BlueprintType)
struct FBossAbilityStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FGameplayTag AbilityTag; // Tag to identify the gameplay ability

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FString AbilityName; // User-friendly name for the ability

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Cooldown; // Cooldown of the ability

    // Add other relevant properties like damage, range, etc. if needed
};
