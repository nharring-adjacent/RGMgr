from ue4helpers import ProjectPackager, VersionHelpers
import os

# Get the project root directory from an environment variable
project_root = os.environ.get("GITHUB_WORKSPACE")
if not project_root:
    raise Exception("GITHUB_WORKSPACE environment variable not set")

# Create a ProjectPackager instance
packager = ProjectPackager(
    root=project_root,
    version=VersionHelpers.from_git_commit(),
    archive='{name}-{version}-{platform}',
    strip_debug=False,
    strip_manifests=False
)

# Clean any previous build artifacts
packager.clean()

# Package the project for Win64, Linux, and Mac
# You may want to customize these platforms based on your needs
packager.package_platform("Win64", "Development")
packager.package_platform("Linux", "Development")
packager.package_platform("Mac", "Development")

# Compress the packaged distributions
packager.archive_platform("Win64")
packager.archive_platform("Linux")
packager.archive_platform("Mac")


print("Project packaged successfully!")
