// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VoiceCommandComponent.generated.h"

// Delegate that will be broadcast when speech is recognized (or simulated)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceCommandRecognized, const FString&, RecognizedText);

/**
 * UVoiceCommandComponent
 *
 * A component responsible for listening to voice input and broadcasting recognized speech.
 * Actual speech recognition capabilities are platform-dependent and would require
 * specific SDK integrations (e.g., Windows Speech Recognition, platform-specific mobile APIs).
 * This component provides the interface and a simulation function for testing.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAIDINGGUILDMANAGER_API UVoiceCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVoiceCommandComponent();

	/** Broadcasts when speech has been recognized (or simulated). */
	UPROPERTY(BlueprintAssignable, Category = "Voice Commands")
	FOnVoiceCommandRecognized OnVoiceCommandRecognized;

	/**
	 * Starts listening for voice commands.
	 * Placeholder implementation. Actual implementation would interact with a speech recognition system.
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice Commands")
	virtual void StartListening();

	/**
	 * Stops listening for voice commands.
	 * Placeholder implementation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice Commands")
	virtual void StopListening();

	/**
	 * Simulates that speech has been recognized. Useful for testing without actual voice input.
	 * @param SimulatedText The text that is pretended to be recognized.
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice Commands")
	void SimulateRecognizedSpeech(const FString& SimulatedText);

protected:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

public:
	//~ Begin UActorComponent Interface
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface
};
