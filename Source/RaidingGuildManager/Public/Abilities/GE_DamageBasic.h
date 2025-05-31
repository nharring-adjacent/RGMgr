// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_DamageBasic.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API UGE_DamageBasic : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_DamageBasic();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float DamageAmount;
};
