// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaidDiagramWidget.generated.h"

class URaidPlanData; // Forward declaration

/**
 * URaidDiagramWidget
 *
 * This UMG widget is designed to allow users to draw paths on a 2D diagram.
 * The actual drawing and input handling are expected to be implemented in the UMG Blueprint editor.
 *
 * How to use in UMG Blueprint:
 * 1. Create a Blueprint based on this C++ class.
 * 2. Input Handling:
 *    - Override `OnMouseButtonDown` in your UMG Blueprint.
 *    - Inside `OnMouseButtonDown`:
 *        - Get the local mouse position using `GetLocalPosition(MouseEvent.PointerIndex)`.
 *        - Determine the `ActionTag` (e.g., from a dropdown or radio buttons in the UI for "Move", "Attack")
 *        - Determine the `TargetID` (e.g., from a text input or by clicking on a representation of a target if "Attack" is selected).
 *        - If it's the start of a new path (e.g., Left Mouse Button click):
 *            Call `StartNewPath`, passing the local mouse position, ActionTag, and TargetID.
 *        - For subsequent points (if drawing point-by-point, e.g., Right Mouse Button or Shift+Left Click):
 *            Call `HandleMouseDown`, passing the local mouse position, ActionTag, and TargetID.
 *        - Alternatively, for continuous line drawing (likely for "Move" actions only):
 *            - On `OnMouseButtonDown`, call `StartNewPath` with a "Move" action.
 *            - Override `OnMouseMove`. If the mouse button is still pressed (check via a boolean flag set in OnMouseButtonDown/Up),
 *              get the local mouse position and call `HandleMouseDown`.
 *    - Override `OnMouseButtonUp` to clear any "mouse button pressed" flags if used for continuous drawing.
 * 3. Path Visualization:
 *    - Override the `OnPaint` function in your UMG Blueprint.
 *    - Inside `OnPaint`:
 *        - Get the path points using `GetPlanData()->GetPathPoints()`.
 *        - Iterate through the points.
 *        - Use `Draw Line` nodes (from the Paint Context) to draw segments between consecutive points.
 *        - Optionally, draw small circles or boxes at each point using `Draw Box` or by adding small image widgets dynamically to a CanvasPanel.
 * 4. Finalizing Path:
 *    - Add a Button widget (e.g., named "Finalize Path Button") to your UMG Blueprint.
 *    - In the `OnClicked` event for this button, call the `FinalizePath` function from this C++ class.
 * 5. Plan Data Access:
 *    - The `PlanData` UObject holding the path points is exposed. You can bind to its properties or call its functions
 *      (like `GetPathPoints`) directly from your UMG Blueprint logic for display purposes.
 */
UCLASS()
class RAIDINGGUILDMANAGER_API URaidDiagramWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URaidDiagramWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UUserWidget Interface
	// virtual void NativeConstruct() override; // Example if needed
	//~ End UUserWidget Interface

	/** The data object that stores the raid plan path being drawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "Raid Plan")
	URaidPlanData* PlanData;

	/**
	 * Call this when the user initiates a new path for a specific character or role.
	 * Clears any existing path for that character/role and adds the initial point.
	 *
	 * In UMG Blueprint:
	 * - The CharacterOrRoleID should be determined by the UI. For example, the user might select a character
	 *   from a dropdown list, or the UI might have a dedicated section for each character's plan.
	 * - This ID (e.g., "TankCharacter_BP_0", "HealerRole", "DPS_Player1") is then passed to this function.
	 * - The UMG Blueprint should manage a list of available/valid CharacterOrRoleIDs, potentially populated
	 *   from game data (e.g., RosterManager or current party composition).
	 *
	 * @param CharacterOrRoleID Identifier for the character or role this path belongs to.
	 * @param InitialPoint The 2D location of the first point (typically from mouse input).
	 * @param Action The gameplay tag representing the action at this point (e.g., "Action.Move", "Action.Attack").
	 * @param TargetID Optional identifier for the target of the action (e.g., "Boss", "Raider3").
	 */
	UFUNCTION(BlueprintCallable, Category = "Raid Plan Input")
	void StartNewPath(const FString& CharacterOrRoleID, const FVector2D& InitialPoint, FGameplayTag Action, const FString& TargetID = TEXT(""));

	/**
	 * Call this to add a subsequent point to the current path for a specific character or role.
	 *
	 * In UMG Blueprint:
	 * - Similar to StartNewPath, the CharacterOrRoleID should be provided based on the current UI context
	 *   (e.g., which character's path is actively being drawn).
	 *
	 * @param CharacterOrRoleID Identifier for the character or role this path belongs to.
	 * @param LocalPosition The 2D location of the point (typically from mouse input).
	 * @param Action The gameplay tag representing the action at this point.
	 * @param TargetID Optional identifier for the target of the action.
	 */
	UFUNCTION(BlueprintCallable, Category = "Raid Plan Input")
	void HandleMouseDown(const FString& CharacterOrRoleID, const FVector2D& LocalPosition, FGameplayTag Action, const FString& TargetID = TEXT(""));

	/**
	 * Call this when the user indicates the path is complete (e.g., clicks a 'Finalize' button).
	 * This finalizes the entire URaidPlanData object, which may now contain multiple paths
	 * for different characters/roles. This PlanData object can then be retrieved using GetPlanData()
	 * and passed, for example, to a RaidLeaderAIController to distribute the individual paths.
	 * For MVP, this logs the path. Future: could broadcast an event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Raid Plan Input")
	void FinalizePath();

	/** Returns the PlanData object. */
	UFUNCTION(BlueprintPure, Category = "Raid Plan")
	URaidPlanData* GetPlanData() const { return PlanData; }
};
