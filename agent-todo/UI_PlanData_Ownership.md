# UI - PlanData Direct Pointer Ownership Risk in RosterManager

## Justification

`URosterManager` (presumably a UObject or Actor class) storing a direct raw pointer to `PlanData` (type `FPlanData*`), where `PlanData` is owned by `URaidDiagramWidget` (a UMG widget), poses a significant risk of dangling pointers. UMG widgets can be destroyed or reconstructed (e.g., on level changes, UI visibility changes, or even layout updates), while the `URosterManager` might persist. If the `URaidDiagramWidget` is destroyed, the `PlanData` it owns will be deallocated, and `URosterManager`'s pointer will become dangling, leading to crashes or undefined behavior upon access. The cost of not fixing this is high instability and potential for hard-to-debug crashes related to UI interactions or lifecycle events.

## Problem Description

The `URosterManager` holds a pointer `FPlanData* CurrentPlanData;`. This pointer is likely set from an instance of `FPlanData` that is a member variable of a `URaidDiagramWidget`.

```cpp
// Hypothetical snippet from URosterManager.h
// Forward declare FPlanData if its definition is elsewhere
struct FPlanData;

class URosterManager : public UObject // Or AActor
{
    // ...
    FPlanData* CurrentPlanData; // Raw pointer to data owned by a widget

public:
    void SetCurrentPlan(FPlanData* NewPlan) { CurrentPlanData = NewPlan; }
    // ... other methods that might use CurrentPlanData ...
};

// Hypothetical snippet from URaidDiagramWidget.h
UCLASS()
class URaidDiagramWidget : public UUserWidget
{
    // ...
    FPlanData MyPlanDataInstance; // Actual instance of FPlanData

public:
    FPlanData* GetPlanData() { return &MyPlanDataInstance; }
    void SomeFunctionThatUpdatesRosterManager()
    {
        // URosterManager* RosterManager = GetGameInstance()->GetSubsystem<URosterManager>(); // Example access
        // if (RosterManager)
        // {
        //    RosterManager->SetCurrentPlan(GetPlanData()); // Problem: MyPlanDataInstance lifetime tied to this widget
        // }
    }

    // virtual void NativeDestruct() override; // When this is called, MyPlanDataInstance is gone
};
```

## Agent Task

Refactor the ownership or access model for `PlanData` to prevent `URosterManager` from holding a dangling pointer. Options include copying the data, using `TWeakObjectPtr` if `PlanData` becomes a `UObject`, or having `URosterManager` request the data from `URaidDiagramWidget` on demand rather than caching a raw pointer.

## Sub-Tasks

- [ ] **Option 1 (Preferred if `FPlanData` is a simple struct): Copy Data**
    - [ ] Change `URosterManager::CurrentPlanData` from `FPlanData*` to `FPlanData` (direct instance) or `TOptional<FPlanData>`.
    - [ ] Modify `URosterManager::SetCurrentPlan` to take `const FPlanData& NewPlan` or `FPlanData NewPlan` and copy the data.
    - [ ] This ensures `URosterManager` has its own copy and is not affected by `URaidDiagramWidget`'s lifecycle.
- [ ] **Option 2 (If `FPlanData` must be shared and is complex, consider making it a UObject): Use Weak Pointer (if FPlanData becomes UObject)**
    - [ ] This would require `FPlanData` to be a `USTRUCT` marked as `BlueprintType` at least, or better, a `UObject` itself. This is a larger refactor.
    - [ ] If `PlanData` becomes a `UObject` (e.g., `UPlanDataObject`), then `URosterManager` can hold a `TWeakObjectPtr<UPlanDataObject> CurrentPlanData;`.
    - [ ] `URaidDiagramWidget` would also hold a `UPlanDataObject*`.
    - [ ] `URosterManager` would need to check `IsValid()` on the weak pointer before access.
- [ ] **Option 3: On-Demand Access (If data doesn't need to be cached in RosterManager)**
    - [ ] Remove `CurrentPlanData` from `URosterManager`.
    - [ ] Modify functions in `URosterManager` that need `PlanData` to take it as a parameter, or to fetch it from `URaidDiagramWidget` (or another authoritative source) each time they need it. This might involve `URosterManager` having a reference to `URaidDiagramWidget` (potentially risky itself if not managed well, e.g. using `TWeakObjectPtr<URaidDiagramWidget>`).
- [ ] Analyze the usage of `CurrentPlanData` in `URosterManager` to determine the best approach. Copying is often safest for non-UObject data.
- [ ] Implement the chosen solution.
- [ ] Test UI interactions, especially those involving the `URaidDiagramWidget` and `URosterManager`, to ensure data consistency and no crashes. Pay attention to scenarios where the widget might be created/destroyed.

## Files to Examine/Modify

- `Source/RaidGame/Public/UI/RosterManager.h` (or equivalent path)
- `Source/RaidGame/Private/UI/RosterManager.cpp`
- `Source/RaidGame/Public/UI/RaidDiagramWidget.h`
- `Source/RaidGame/Private/UI/RaidDiagramWidget.cpp`
- The definition of `FPlanData` (likely in `RaidDiagramWidget.h` or a separate types file).
