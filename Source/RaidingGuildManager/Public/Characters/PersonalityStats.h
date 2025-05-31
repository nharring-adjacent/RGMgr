#pragma once

#include "CoreMinimal.h"
#include "PersonalityStats.generated.h"

USTRUCT(BlueprintType)
struct FPersonalityStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
    float Focus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
    float Obedience;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
    float ClassSkill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
    float Recruitability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality Stats")
    float Morale;

    // Constructor to initialize with default values
    FPersonalityStats()
        : Focus(50.0f)
        , Obedience(50.0f)
        , ClassSkill(50.0f)
        , Recruitability(50.0f)
        , Morale(50.0f)
    {
    }
};
