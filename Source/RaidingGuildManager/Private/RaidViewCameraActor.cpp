#include "RaidViewCameraActor.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARaidViewCameraActor::ARaidViewCameraActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true; // Not needed for a static camera

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("RaidViewCamera"));
	RootComponent = CameraComponent; // Make the camera component the root

	// Set camera position and rotation for an isometric/top-down view
	// Looking towards positive X, from a height (Z) and slightly offset in Y or X if desired.
	// This example sets it up at a distance, looking down at a 60-degree angle.
	CameraComponent->SetRelativeLocation(FVector(-800.f, 0.f, 1200.f)); // Adjust X, Y, Z for desired distance and height
	CameraComponent->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));   // Pitch down, no roll or yaw initially

	// Other useful camera settings you might consider:
	// CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	// CameraComponent->OrthoWidth = 2048.f; // Adjust for desired orthographic view width
	// CameraComponent->bConstrainAspectRatio = true;
	// CameraComponent->FieldOfView = 90.f; // For perspective
}

// Called when the game starts or when spawned
/*
void ARaidViewCameraActor::BeginPlay()
{
	Super::BeginPlay();

}
*/

// Called every frame
/*
void ARaidViewCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
