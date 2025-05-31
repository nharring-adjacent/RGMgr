// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RosterDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FRaiderInfoUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	FString RaiderName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roster")
	FString RaiderRole;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	int32 RaiderID;

	FRaiderInfoUIData()
		: RaiderName(TEXT("Unknown Raider"))
		, RaiderRole(TEXT("Unknown Role"))
		, RaiderID(-1)
	{}

	FRaiderInfoUIData(const FString& InName, const FString& InRole, int32 InID)
		: RaiderName(InName)
		, RaiderRole(InRole)
		, RaiderID(InID)
	{}
};
