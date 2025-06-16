# CI - Docker Image Inconsistency in CI Pipeline

## Justification

The main CI pipeline (`main.yml` or similar) uses Epic Games' official Docker image for Unreal Engine, but a custom `Dockerfile` also exists in the repository (e.g., in `.github/images/Dockerfile` or `Source/Docker/Dockerfile`). This indicates a potential inconsistency: either the custom Dockerfile is outdated and unused, or the CI is not leveraging a potentially more tailored build environment defined in the custom Dockerfile. Using an inconsistent or non-optimal Docker image can lead to slower CI build times (if the custom image has pre-cached dependencies), build failures if specific SDKs or tools are missing from the Epic image but included in the custom one, or a drift between local development environments (if devs use the custom Dockerfile) and CI. The cost of not fixing this is potential CI inefficiencies, build brittleness, and "works on my machine" issues.

## Problem Description

The CI configuration file (e.g., `.github/workflows/main.yml`) specifies a Docker image like `ghcr.io/epicgames/unreal-engine:5.X.X` for its jobs. However, a file named `Dockerfile` exists elsewhere in the project, defining its own image setup, possibly based on an Epic image but adding more layers or tools.

```yaml
# Hypothetical snippet from .github/workflows/main.yml
name: Main CI

on: [push, pull_request]

jobs:
  build:
    name: Build Game
    runs-on: ubuntu-latest
    container:
      image: ghcr.io/epicgames/unreal-engine:5.2.0-linux # Using Epic's image
    steps:
      - name: Checkout code
        uses: actions/checkout@v3
      # ... build steps ...
```

```dockerfile
# Hypothetical snippet from .github/images/Dockerfile or Source/Docker/Dockerfile
FROM ghcr.io/epicgames/unreal-engine:5.2.0-linux

# Customizations:
USER root
RUN apt-get update && apt-get install -y some-required-package
# Maybe pre-caches some project specific dependencies
USER unreal
```
This custom `Dockerfile` seems unused by the main CI pipeline.

## Agent Task

Investigate the purpose of the custom `Dockerfile`. If it's intended to be the primary build environment, update the CI pipeline to build and use this custom Docker image. If it's outdated or deprecated, remove it to avoid confusion.

## Sub-Tasks

- [ ] Locate the custom `Dockerfile` in the repository.
- [ ] Examine the contents of the `Dockerfile` to understand its purpose and what customizations it provides compared to the base Epic Games image.
- [ ] Check repository history or documentation to see if there's context for why this `Dockerfile` was created.
- [ ] **Decision Point:**
    - [ ] **If the custom `Dockerfile` is current and beneficial (e.g., includes necessary dependencies, could speed up CI):**
        - [ ] Modify the CI workflow (`main.yml`) to build this Docker image and tag it (e.g., `projectname-ci:latest`).
        - [ ] Update the CI jobs to use this custom-built image instead of the generic Epic Games image.
        - [ ] Consider publishing the custom image to a container registry (like GHCR) to speed up subsequent CI runs by avoiding rebuilds unless the Dockerfile changes.
    - [ ] **If the custom `Dockerfile` is outdated, an experiment, or its benefits are unclear and the Epic image works fine:**
        - [ ] Confirm with the development team if it's safe to remove.
        - [ ] If confirmed, delete the custom `Dockerfile` and any related image-building scripts.
        - [ ] Ensure the CI pipeline (`main.yml`) explicitly uses a well-defined version of the Epic Games image.
- [ ] Document the chosen Docker image strategy for CI.
- [ ] Test the CI pipeline thoroughly after making changes.

## Files to Examine/Modify

- `.github/workflows/main.yml` (or the primary CI configuration file)
- The custom `Dockerfile` (e.g., `.github/images/Dockerfile`, `Source/Docker/Dockerfile`, or other locations)
- Any scripts related to building or publishing the custom Docker image.
