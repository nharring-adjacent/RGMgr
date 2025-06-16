# CI - Build Configuration and Platform Rigidity

## Justification

The current CI pipeline builds the project for a fixed set of platforms (e.g., Win64, Linux, Mac) and always uses the "Development" build configuration. This lacks flexibility for several reasons: "Shipping" configurations are needed for release builds; developers might want to test or build for specific platforms only to save time; or different branches might require different build targets (e.g., a release branch targeting Shipping, while a feature branch targets Development). The cost of not fixing this is inefficient use of CI resources (building unnecessary platforms/configurations), longer CI runtimes, and a manual process for creating release (Shipping) builds.

## Problem Description

The CI workflow (e.g., `main.yml`) likely has hardcoded platform lists and build configurations in its build commands.

```yaml
# Hypothetical snippet from .github/workflows/main.yml
jobs:
  build:
    name: Build Game
    # ...
    steps:
      # ...
      - name: Build Project
        run: |
          Engine/Build/BatchFiles/RunUAT.sh BuildCookRun \
            -project="$GITHUB_WORKSPACE/RaidGame.uproject" \
            -platform=Win64+Linux+Mac \ # Hardcoded platforms
            -clientconfig=Development \   # Hardcoded configuration
            -serverconfig=Development \   # Hardcoded configuration
            -cook \
            -build \
            -stage \
            -pak \
            -archive \
            -archivedirectory="$GITHUB_WORKSPACE/Builds"
```
This setup doesn't allow for easy changes to target platforms or configurations without modifying the workflow file directly. It also doesn't produce "Shipping" builds necessary for actual game releases.

## Agent Task

Parameterize the CI build process to allow dynamic selection of target platforms and build configurations. Implement logic to build "Shipping" configurations, potentially based on branch name or manual triggers.

## Sub-Tasks

- [ ] **Parameterize Platforms & Configurations:**
    - [ ] Modify the CI workflow (`main.yml`) to accept inputs for platforms and configurations (e.g., using `workflow_dispatch` inputs, or by setting environment variables based on branch names or tags).
    - [ ] Update the `RunUAT.sh` command to use these input variables for `-platform`, `-clientconfig`, and `-serverconfig`.
- [ ] **Support "Shipping" Builds:**
    - [ ] Ensure that "Shipping" is a valid option for build configurations.
    - [ ] Implement logic to determine when to build "Shipping". Examples:
        - On pushes to a specific branch (e.g., `release/*`, `main`).
        - When a specific tag is pushed (e.g., `v*.*.*`).
        - Via a manual `workflow_dispatch` trigger with a "configuration" dropdown.
- [ ] **Default Behavior:**
    - [ ] Define sensible default platforms and configurations if no inputs are provided (e.g., "Development" config for "Win64" platform on typical PR builds).
- [ ] **Documentation:**
    - [ ] Document how to trigger builds with specific platforms and configurations.
- [ ] **Testing:**
    - [ ] Test building with "Development" configuration for a single platform.
    - [ ] Test building with "Shipping" configuration for relevant release platforms.
    - [ ] Test the default behavior on a PR.
- [ ] Consider splitting build jobs: If building for multiple platforms/configs takes too long, consider creating separate matrix jobs for each combination, or parallel jobs.

## Files to Examine/Modify

- `.github/workflows/main.yml` (or the primary CI configuration file)
- Any scripts called by the workflow that might also contain hardcoded platform/config information.
- Potentially `Project.Build.cs` or `Target.cs` files if they need adjustments for Shipping builds, though usually these are standard.
