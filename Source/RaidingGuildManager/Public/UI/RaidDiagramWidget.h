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
	 * Call this when the user initiates a new path (e.g., first click).
	 * Clears any existing path and adds the initial point with its associated action.
	 * @param InitialPoint The 2D location of the first point.
	 * @param Action The gameplay tag representing the action at this point (e.g., "Action.Move", "Action.Attack").
	 * @param TargetID Optional identifier for the target of the action (e.g., "Boss", "Raider3").
	 */
	UFUNCTION(BlueprintCallable, Category = "Raid Plan Input")
	void StartNewPath(const FVector2D& InitialPoint, FGameplayTag Action, const FString& TargetID = TEXT(""));

	/**
	 * Call this to add a subsequent point to the current path.
	 * This would be called from Blueprint's OnMouseButtonDown (for point-by-point)
	 * or OnMouseMove (for continuous drawing - typically with a "Move" action).
	 * @param LocalPosition The 2D location of the point.
	 * @param Action The gameplay tag representing the action at this point.
	 * @param TargetID Optional identifier for the target of the action.
	 */
	UFUNCTION(BlueprintCallable, Category = "Raid Plan Input")
	void HandleMouseDown(const FVector2D& LocalPosition, FGameplayTag Action, const FString& TargetID = TEXT(""));

	/**
	 * Call this when the user indicates the path is complete (e.g., clicks a 'Finalize' button).
	 * For MVP, this logs the path. Future: could broadcast an event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Raid Plan Input")
	void FinalizePath();

	/** Returns the PlanData object. */
	UFUNCTION(BlueprintPure, Category = "Raid Plan")
	URaidPlanData* GetPlanData() const { return PlanData; }
};
