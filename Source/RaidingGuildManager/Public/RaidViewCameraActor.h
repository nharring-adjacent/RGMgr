#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RaidViewCameraActor.generated.h"

class UCameraComponent;

UCLASS()
class RAIDINGGUILDMANAGER_API ARaidViewCameraActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARaidViewCameraActor();

protected:
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
};
