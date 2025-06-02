// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/RaiderCharacter.h"
#include "Characters/CharacterTypes.h"
#include "TankRaider.generated.h"

/**
 *
 */
UCLASS()
class RAIDINGGUILDMANAGER_API ATankRaider : public ARaiderCharacter
{
	GENERATED_BODY()

public:
	ATankRaider();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
