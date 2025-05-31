#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BossStageStruct.h"
#include "BossDataAsset.generated.h"

UCLASS(BlueprintType)
class RAIDINGGUILDMANAGER_API UBossDataAsset : public UPrimaryDataAsset // Changed from UDataAsset to UPrimaryDataAsset for better management
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Details")
    FString BossName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Details")
    UStaticMesh* BossMesh; // Optional: Visual representation

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Details")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Stages")
    TArray<FBossStageStruct> Stages;

    // Optional: Add default abilities if the boss has abilities outside of stages
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Abilities")
    TArray<FBossAbilityStruct> DefaultAbilities;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("BossData", GetFName());
    }
};
