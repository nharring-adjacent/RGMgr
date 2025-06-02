#include "RGMPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Characters/RaiderCharacter.h" // Required for ARaiderCharacter
#include "DrawDebugHelpers.h" // Required for DrawDebugLine
#include "EngineUtils.h" // Required for TActorIterator
#include "Blueprint/UserWidget.h" // Required for UUserWidget
#include "UObject/ConstructorHelpers.h" // Required for FClassFinder
#include "UI/RaiderCommandsWidget.h" // Required for URaiderCommandsWidget
#include "RaidViewCameraActor.h" // Required for ARaidViewCameraActor

ARGMPlayerController::ARGMPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	SelectedRaider = nullptr; // Initialize SelectedRaider
	RaiderCommandsWidgetInstance = nullptr; // Initialize widget instance

	// Attempt to find the RaiderCommandsWidget Blueprint class
	// This path assumes the Blueprint UMG asset WBP_RaiderCommands is created in Content/UI/
	static ConstructorHelpers::FClassFinder<URaiderCommandsWidget> RaiderCommandsWidgetBPClass(TEXT("/Game/UI/WBP_RaiderCommands"));
	if (RaiderCommandsWidgetBPClass.Class != nullptr)
	{
		RaiderCommandsWidgetClass = RaiderCommandsWidgetBPClass.Class;
		UE_LOG(LogTemp, Log, TEXT("Successfully found WBP_RaiderCommands class at %s"), *RaiderCommandsWidgetBPClass.GetPathName());
	}
	else
	{
		RaiderCommandsWidgetClass = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Could not find WBP_RaiderCommands class. Ensure it exists at /Game/UI/WBP_RaiderCommands"));
	}
}

void ARGMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (RaiderCommandsWidgetClass)
	{
		RaiderCommandsWidgetInstance = CreateWidget<URaiderCommandsWidget>(this, RaiderCommandsWidgetClass);
		if (RaiderCommandsWidgetInstance)
		{
			RaiderCommandsWidgetInstance->AddToViewport();
			RaiderCommandsWidgetInstance->SetVisibility(ESlateVisibility::Hidden); // Initially hidden
			UE_LOG(LogTemp, Log, TEXT("RaiderCommandsWidgetInstance created and added to viewport, initially hidden."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create RaiderCommandsWidgetInstance."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RaiderCommandsWidgetClass is null. Cannot create RaiderCommandsWidgetInstance."));
	}

	// Spawn and set the Raid View Camera
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		// Spawn location and rotation can be adjusted if needed, but camera itself has its view settings.
		// Spawning at ZeroVector, ZeroRotator means the camera actor itself is at origin,
		// its internal CameraComponent's relative transform provides the actual view.
		ARaidViewCameraActor* MyRaidCamera = World->SpawnActor<ARaidViewCameraActor>(ARaidViewCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (MyRaidCamera)
		{
			SetViewTarget(MyRaidCamera);
			UE_LOG(LogTemp, Log, TEXT("ARGMPlayerController: Successfully spawned ARaidViewCameraActor and set as view target."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ARGMPlayerController: Failed to spawn ARaidViewCameraActor."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARGMPlayerController: GetWorld() returned null in BeginPlay. Cannot spawn camera."));
	}
}

void ARGMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &ARGMPlayerController::OnLeftMouseClick);
	}
}

void ARGMPlayerController::OnLeftMouseClick()
{
	FVector MouseLocation, MouseDirection;
	if (DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
	{
		FVector TraceEnd = MouseLocation + (MouseDirection * 10000.0f); // 10000 is the trace distance

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // Ignore player controller itself

		// Perform the line trace
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			MouseLocation,
			TraceEnd,
			ECC_Visibility, // Trace against the Visibility channel
			CollisionParams
		);

		// Optional: Draw debug line to visualize the trace
		DrawDebugLine(
			GetWorld(),
			MouseLocation,
			TraceEnd,
			FColor::Red,
			false, 5.0f, 0,
			1.0f
		);

		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				ARaiderCharacter* ClickedRaider = Cast<ARaiderCharacter>(HitActor);

				// Deselect all raiders first
				for (TActorIterator<ARaiderCharacter> It(GetWorld()); It; ++It)
				{
					ARaiderCharacter* Raider = *It;
					if (Raider)
					{
						Raider->SetSelected(false);
					}
				}

				if (ClickedRaider)
				{
					ClickedRaider->SetSelected(true);
					SelectedRaider = ClickedRaider;
					UE_LOG(LogTemp, Log, TEXT("Selected RaiderCharacter: %s"), *SelectedRaider->GetName());
					if (RaiderCommandsWidgetInstance)
					{
						RaiderCommandsWidgetInstance->SetVisibility(ESlateVisibility::Visible);
					}
				}
				else
				{
					SelectedRaider = nullptr; // Clicked on something else, so deselect
					UE_LOG(LogTemp, Log, TEXT("Clicked on Actor: %s (Not a RaiderCharacter). Current selection cleared."), *HitActor->GetName());
					if (RaiderCommandsWidgetInstance)
					{
						RaiderCommandsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
			else // Hit something, but it's not an actor (e.g. landscape)
			{
				// Deselect all raiders if we click on non-actor
				for (TActorIterator<ARaiderCharacter> It(GetWorld()); It; ++It)
				{
					ARaiderCharacter* Raider = *It;
					if (Raider)
					{
						Raider->SetSelected(false);
					}
				}
				SelectedRaider = nullptr;
				UE_LOG(LogTemp, Log, TEXT("Mouse click hit something, but not an actor. Current selection cleared."));
				if (RaiderCommandsWidgetInstance)
				{
					RaiderCommandsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		else // Clicked on empty space
		{
			// Deselect all raiders if we click on empty space
			for (TActorIterator<ARaiderCharacter> It(GetWorld()); It; ++It)
			{
				ARaiderCharacter* Raider = *It;
				if (Raider)
				{
					Raider->SetSelected(false);
				}
			}
			SelectedRaider = nullptr;
			UE_LOG(LogTemp, Log, TEXT("Mouse click did not hit any actor. Current selection cleared."));
			if (RaiderCommandsWidgetInstance)
			{
				RaiderCommandsWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to deproject mouse position to world."));
	}
}
