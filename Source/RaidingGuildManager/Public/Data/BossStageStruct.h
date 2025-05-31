#pragma once

#include "CoreMinimal.h"
#include "BossAbilityStruct.h"
#include "GameplayTagContainer.h" // For environment change tags
#include "BossStageStruct.generated.h"

UENUM(BlueprintType)
enum class EStageTriggerCondition : uint8
{
    HealthPercentage UMETA(DisplayName = "Health Percentage"),
    Timer UMETA(DisplayName = "Timer")
};

USTRUCT(BlueprintType)
struct FBossStageStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    FString StageName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    TArray<FBossAbilityStruct> Abilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Trigger")
    EStageTriggerCondition TriggerCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Trigger", meta = (EditCondition = "TriggerCondition == EStageTriggerCondition::HealthPercentage"))
    float TriggerHealthPercentage; // From 0.0 to 1.0

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Trigger", meta = (EditCondition = "TriggerCondition == EStageTriggerCondition::Timer"))
    float TriggerTimerSeconds; // Time in seconds from the start of the fight or previous stage

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Environment")
    FGameplayTag EnvironmentChangeTag; // Tag to identify an environment change

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    bool bRepeatable; // Can this stage be re-entered if conditions are met again? (e.g. healing above threshold)
};
