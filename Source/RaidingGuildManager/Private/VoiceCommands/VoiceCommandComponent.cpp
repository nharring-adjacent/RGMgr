// Copyright Epic Games, Inc. All Rights Reserved.

#include "VoiceCommands/VoiceCommandComponent.h"

UVoiceCommandComponent::UVoiceCommandComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// PrimaryComponentTick.bCanEverTick = true; // Typically false unless actively polling
	PrimaryComponentTick.bCanEverTick = false;
}

void UVoiceCommandComponent::BeginPlay()
{
	Super::BeginPlay();
	// Initialization for a real speech recognition system might go here.
	// For example, initializing a connection to a voice SDK.
}

void UVoiceCommandComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up for a real speech recognition system.
	// StopListening(); // Ensure listening is stopped
	Super::EndPlay(EndPlayReason);
}

void UVoiceCommandComponent::StartListening()
{
	UE_LOG(LogTemp, Log, TEXT("VoiceCommandComponent: StartListening (Placeholder - No actual speech recognition implemented)"));
	// In a real implementation:
	// if (SpeechRecognitionSystem && !SpeechRecognitionSystem->IsListening())
	// {
	//     SpeechRecognitionSystem->Start();
	// }
}

void UVoiceCommandComponent::StopListening()
{
	UE_LOG(LogTemp, Log, TEXT("VoiceCommandComponent: StopListening (Placeholder)"));
	// In a real implementation:
	// if (SpeechRecognitionSystem && SpeechRecognitionSystem->IsListening())
	// {
	//     SpeechRecognitionSystem->Stop();
	// }
}

void UVoiceCommandComponent::SimulateRecognizedSpeech(const FString& SimulatedText)
{
	if (SimulatedText.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("VoiceCommandComponent: SimulateRecognizedSpeech called with empty text."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("VoiceCommandComponent: Simulated speech recognized: \"%s\""), *SimulatedText);

	// Broadcast the recognized text so other systems can process it.
	OnVoiceCommandRecognized.Broadcast(SimulatedText);
}
