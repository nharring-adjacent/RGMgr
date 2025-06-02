// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/RaiderAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h" // Required for UBehaviorTree
#include "Characters/RGMCharacterBase.h" // For setting SelfActor

// Define static const FName members
const FName ARaiderAIController::TargetActorKeyName = FName("TargetActor");
const FName ARaiderAIController::SelfActorKeyName = FName("SelfActor");
const FName ARaiderAIController::PathTargetLocationKeyName = FName("PathTargetLocation");
const FName ARaiderAIController::HasPathKeyName = FName("HasPath");
const FName ARaiderAIController::PathCurrentIndexKeyName = FName("PathCurrentIndex");
const FName ARaiderAIController::PathCompletedKeyName = FName("PathCompleted");
#include "Kismet/GameplayStatics.h"   // For GetActorOfClass
#include "Characters/BossCharacter.h" // For ABossCharacter
#include "Planning/RaidPlanDataTypes.h" // For FRaidPlanPointData

const FName ARaiderAIController::VoiceCommandTagKeyName = FName("VoiceCommandTag");
const FName ARaiderAIController::VoiceCommandTargetKeyName = FName("VoiceCommandTarget");
const FName ARaiderAIController::VoiceCommandLocationKeyName = FName("VoiceCommandLocation");
const FName ARaiderAIController::CurrentActionTagKeyName = FName("CurrentActionTag");
const FName ARaiderAIController::CurrentActionTargetKeyName = FName("CurrentActionTarget");


ARaiderAIController::ARaiderAIController()
#include "GameplayTagContainer.h" // For FGameplayTag in ExecuteVoiceCommand (though already in .h, good for cpp too)
// Potentially GameplayTagsManager if needed directly, but SetValueAsTag handles it.

ARaiderAIController::ARaiderAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
#include "Characters/RGMCharacterBase.h" // For ARGMCharacterBase cast in OnPossess

ARaiderAIController::ARaiderAIController()
#include "GameplayTagContainer.h" // For FGameplayTag in ExecuteVoiceCommand (though already in .h, good for cpp too)
// Potentially GameplayTagsManager if needed directly, but SetValueAsTag handles it.

ARaiderAIController::ARaiderAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	CurrentRaidPathPoints.Empty();
}

void ARaiderAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr)
	{
		return;
	}

	ARGMCharacterBase* RGMCharacter = Cast<ARGMCharacterBase>(InPawn);
	if (RGMCharacter)
	{
		if (RGMCharacter->GetAbilitySystemComponent())
		{
			RGMCharacter->InitializeAttributes();
			RGMCharacter->GiveDefaultAbilities();
			UE_LOG(LogTemp, Log, TEXT("RaiderAIC %s: Initialized attributes and abilities for %s"), *GetNameSafe(this), *InPawn->GetName());
		}

		if (BlackboardComponent && BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
		{
			// InitializeBlackboard(*BlackboardComponent, *BehaviorTreeAsset->BlackboardAsset);
			// BlackboardComponent->SetValueAsObject(SelfActorKeyName, Character);
			// // Initialize path keys to a default "no path" state
			// BlackboardComponent->SetValueAsBool(HasPathKeyName, false);
			// BlackboardComponent->SetValueAsInt(PathCurrentIndexKeyName, -1);
			// BlackboardComponent->SetValueAsBool(PathCompletedKeyName, true);
			// // Initialize voice command keys
			// BlackboardComponent->SetValueAsTag(VoiceCommandTagKeyName, FGameplayTag::EmptyTag);
			// BlackboardComponent->SetValueAsObject(VoiceCommandTargetKeyName, nullptr);
			// BlackboardComponent->SetValueAsVector(VoiceCommandLocationKeyName, FVector::ZeroVector);
			// // Initialize new action keys
			// BlackboardComponent->SetValueAsTag(CurrentActionTagKeyName, FGameplayTag::RequestGameplayTag(FName("Action.Move"))); // Default to move
			// BlackboardComponent->SetValueAsObject(CurrentActionTargetKeyName, nullptr);

			// RunBehaviorTree(BehaviorTreeAsset);
			// UE_LOG(LogTemp, Log, TEXT("RaiderAIController possessed %s and started behavior tree %s"), *InPawn->GetName(), *BehaviorTreeAsset->GetName());
			UE_LOG(LogTemp, Log, TEXT("RaiderAIController possessed %s. Behavior Tree execution is DISABLED for UtilityAI."), *InPawn->GetName());
		}
		else
		{
			if(!BehaviorTreeAsset) UE_LOG(LogTemp, Warning, TEXT("RaiderAIController has no BehaviorTreeAsset set for %s!"), *InPawn->GetName());
			if(BehaviorTreeAsset && !BehaviorTreeAsset->BlackboardAsset) UE_LOG(LogTemp, Warning, TEXT("RaiderAIController's BehaviorTreeAsset %s has no BlackboardAsset set for %s!"), *BehaviorTreeAsset->GetName(), *InPawn->GetName());
			if(!BlackboardComponent) UE_LOG(LogTemp, Warning, TEXT("RaiderAIController has no BlackboardComponent for %s!"), *InPawn->GetName());
		}
	}
}

void ARaiderAIController::SetPathToFollow(const TArray<FRaidPlanPointData>& PathPoints)
{
	UE_LOG(LogTemp, Log, TEXT("RaiderAIC %s: SetPathToFollow called with %d points."), *GetNameSafe(GetPawn()), PathPoints.Num());
	CurrentRaidPathPoints = PathPoints;

	if (BlackboardComponent)
	{
		if (PathPoints.Num() > 0)
		{
			for (int32 i = 0; i < PathPoints.Num(); ++i)
			{
				const FRaidPlanPointData& Point = PathPoints[i];
				UE_LOG(LogTemp, Log, TEXT("RaiderAIC %s: Path Point %d: Loc=%s, Action=%s, TargetID=%s"),
					*GetNameSafe(GetPawn()), i, *Point.Location.ToString(), *Point.ActionTag.ToString(), *Point.TargetIdentifier);
			}

			const FRaidPlanPointData& FirstPoint = PathPoints[0];
			FVector FirstTargetLocationVec = FVector(FirstPoint.Location.X, FirstPoint.Location.Y, 0.0f);

			BlackboardComponent->SetValueAsBool(HasPathKeyName, true);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to true"), *GetNameSafe(GetPawn()), *HasPathKeyName.ToString());

			BlackboardComponent->SetValueAsInt(PathCurrentIndexKeyName, 0);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to 0"), *GetNameSafe(GetPawn()), *PathCurrentIndexKeyName.ToString());

			BlackboardComponent->SetValueAsBool(PathCompletedKeyName, false);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to false"), *GetNameSafe(GetPawn()), *PathCompletedKeyName.ToString());

			BlackboardComponent->SetValueAsVector(PathTargetLocationKeyName, FirstTargetLocationVec);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to %s"), *GetNameSafe(GetPawn()), *PathTargetLocationKeyName.ToString(), *FirstTargetLocationVec.ToString());

			BlackboardComponent->SetValueAsTag(CurrentActionTagKeyName, FirstPoint.ActionTag);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to %s"), *GetNameSafe(GetPawn()), *CurrentActionTagKeyName.ToString(), *FirstPoint.ActionTag.ToString());

			ResolveAndSetTarget(FirstPoint.TargetIdentifier, CurrentActionTargetKeyName);
		}
		else
		{
			BlackboardComponent->SetValueAsBool(HasPathKeyName, false);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to false (empty path)"), *GetNameSafe(GetPawn()), *HasPathKeyName.ToString());
			BlackboardComponent->SetValueAsInt(PathCurrentIndexKeyName, -1);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to -1 (empty path)"), *GetNameSafe(GetPawn()), *PathCurrentIndexKeyName.ToString());
			BlackboardComponent->SetValueAsBool(PathCompletedKeyName, true);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to true (empty path)"), *GetNameSafe(GetPawn()), *PathCompletedKeyName.ToString());
			BlackboardComponent->SetValueAsTag(CurrentActionTagKeyName, FGameplayTag::RequestGameplayTag(FName("Action.Move")));
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to Action.Move (empty path)"), *GetNameSafe(GetPawn()), *CurrentActionTagKeyName.ToString());
			BlackboardComponent->SetValueAsObject(CurrentActionTargetKeyName, nullptr);
			UE_LOG(LogTemp, Verbose, TEXT("RaiderAIC %s: BB Key %s set to nullptr (empty path)"), *GetNameSafe(GetPawn()), *CurrentActionTargetKeyName.ToString());
			UE_LOG(LogTemp, Log, TEXT("RaiderAIC %s: SetPathToFollow with an empty path. Path cleared."), *GetNameSafe(GetPawn()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RaiderAIC %s: BlackboardComponent is null, cannot set path values."), *GetNameSafe(GetPawn()));
	}
}

const TArray<FRaidPlanPointData>& ARaiderAIController::GetCurrentPathPoints() const
{
	return CurrentRaidPathPoints;
}

void ARaiderAIController::ResolveAndSetTarget(const FString& TargetID, FName BBKeyName)
{
	AActor* FoundTarget = nullptr;
	if (TargetID.IsEmpty())
	{
		if (BlackboardComponent) BlackboardComponent->SetValueAsObject(BBKeyName, nullptr);
		UE_LOG(LogTemp, Log, TEXT("RaiderAIC %s: TargetID is empty. BB Key %s set to nullptr."), *GetNameSafe(GetPawn()), *BBKeyName.ToString());
		return;
	}

	if (TargetID.Equals(TEXT("Boss"), ESearchCase::IgnoreCase))
	{
		FoundTarget = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());
	}
	// TODO: Implement more complex target resolution logic

	UE_LOG(LogTemp, Log, TEXT("RaiderAIC %s: Resolving TargetID '%s'. Found: %s. Setting BB Key %s."), *GetNameSafe(GetPawn()), *TargetID, *GetNameSafe(FoundTarget), *BBKeyName.ToString());

	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(BBKeyName, FoundTarget);
	}
}


void ARaiderAIController::ExecuteVoiceCommand(FGameplayTag CommandTag, UObject* Target, const FVector& Location)
{
	if (BlackboardComponent)
	{
		// Clear previous command target/location if the new command doesn't set them,
		// or ensure they are explicitly nulled/zeroed if not relevant to the new command.
		// For simplicity, new command overwrites. Consider clearing logic if commands can be partial.

		BlackboardComponent->SetValueAsTag(VoiceCommandTagKeyName, CommandTag);
		BlackboardComponent->SetValueAsObject(VoiceCommandTargetKeyName, Target); // Target can be nullptr
		BlackboardComponent->SetValueAsVector(VoiceCommandLocationKeyName, Location); // Location can be ZeroVector

		FString TargetName = Target ? Target->GetName() : TEXT("None");
		UE_LOG(LogTemp, Log, TEXT("AI Controller %s (Pawn: %s): ExecuteVoiceCommand received. Tag: %s, Target: %s, Location: %s"),
			*GetName(),
			*GetNameSafe(GetPawn()),
			*CommandTag.ToString(),
			*TargetName,
			*Location.ToString());

		// Optional: Force re-evaluation of the BT if it's not set to self-re-evaluate on BB changes.
		// BehaviorTreeComponent->RequestExecution(EBTNodeResult::Aborted); // Or another method to make BT react
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller %s (Pawn: %s): BlackboardComponent is null, cannot execute voice command."),
			*GetName(),
			*GetNameSafe(GetPawn()));
	}
}
