#include "CoreTypes.h"
#include "Tests/AutomationEditorCommon.h" // For editor-context tests
#include "Data/BossDataAsset.h"          // Include your data asset header
#include "Data/BossStageStruct.h"
#include "Data/BossAbilityStruct.h"
#include "GameplayTagsManager.h"       // For creating FGameplayTag

// Basic test to check if UBossDataAsset can be created and holds data
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBossDataAssetCreationTest, "RaidingGuildManager.Tests.Data.BossDataAssetCreation", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FBossDataAssetCreationTest::RunTest(const FString& Parameters)
{
    // Create a UBossDataAsset instance
    UBossDataAsset* TestBossAsset = NewObject<UBossDataAsset>();
    TestNotNull(TEXT("TestBossAsset should not be null after creation."), TestBossAsset);

    if (!TestBossAsset) return false;

    // Assign some data
    TestBossAsset->BossName = TEXT("TestBoss");
    TestBossAsset->MaxHealth = 1000.0f;

    TestEqual(TEXT("BossName should be 'TestBoss'"), TestBossAsset->BossName, FString(TEXT("TestBoss")));
    TestEqual(TEXT("MaxHealth should be 1000.0f"), TestBossAsset->MaxHealth, 1000.0f);

    // Add a stage
    FBossStageStruct Stage1;
    Stage1.StageName = TEXT("Phase 1");
    Stage1.TriggerCondition = EStageTriggerCondition::HealthPercentage;
    Stage1.TriggerHealthPercentage = 0.75f;
    Stage1.bRepeatable = false;

    // Create a gameplay tag for an ability
    FGameplayTag AbilityTag = UGameplayTagsManager::Get().AddTag(FName("Test.Ability.Fireball"));
    // UGameplayTagsManager::Get().RequestGameplayTag(FName("Test.Ability.Fireball"), false); // Alternative if tag should exist

    FBossAbilityStruct Ability1;
    Ability1.AbilityName = TEXT("Fireball");
    Ability1.AbilityTag = AbilityTag;
    Ability1.Cooldown = 10.0f;
    Stage1.Abilities.Add(Ability1);

    TestBossAsset->Stages.Add(Stage1);

    TestEqual(TEXT("Number of stages should be 1"), TestBossAsset->Stages.Num(), 1);
    if (TestBossAsset->Stages.Num() > 0)
    {
        TestEqual(TEXT("Stage 1 name should be 'Phase 1'"), TestBossAsset->Stages[0].StageName, FString(TEXT("Phase 1")));
        TestEqual(TEXT("Stage 1 ability count should be 1"), TestBossAsset->Stages[0].Abilities.Num(), 1);
        if (TestBossAsset->Stages[0].Abilities.Num() > 0)
        {
            TestEqual(TEXT("Ability 1 name should be 'Fireball'"), TestBossAsset->Stages[0].Abilities[0].AbilityName, FString(TEXT("Fireball")));
            TestEqual(TEXT("Ability 1 Tag should match"), TestBossAsset->Stages[0].Abilities[0].AbilityTag, AbilityTag);
        }
    }

    // Test PrimaryAssetId (if applicable, UPrimaryDataAsset)
    FPrimaryAssetId AssetId = TestBossAsset->GetPrimaryAssetId();
    TestEqual(TEXT("PrimaryAssetId Type should be 'BossData'"), AssetId.PrimaryAssetType.ToString(), FString(TEXT("BossData")));
    TestEqual(TEXT("PrimaryAssetId Name should match object name"), AssetId.PrimaryAssetName.ToString(), TestBossAsset->GetFName().ToString());


    // Mark GWorld as dirty if modifying global state like GameplayTagsManager
    // if (GWorld) GWorld->MarkPackageDirty(); // Not strictly needed for NewObject usually

    return true;
}

// Add more tests for:
// - Multiple stages, multiple abilities
// - Different trigger conditions (Timer)
// - Default abilities on UBossDataAsset
// - Edge cases (empty stages, empty abilities array)
