from ue4helpers import ProjectPackager, VersionHelpers
import os

# Get the project root directory from an environment variable
project_root = os.environ.get("GITHUB_WORKSPACE")
if not project_root:
    raise Exception("GITHUB_WORKSPACE environment variable not set")

# Create a ProjectBuild instance
builder = ProjectPackager(project_root, VersionHelpers.from_git_commit())

# Build the project for Win64, Linux, and Mac
# You may want to customize these platforms based on your needs
builder.package(args=["Development", "-platform=Win64"])
builder.package(args=["Development", "-platform=Linux"])
builder.package(args=["Development", "-platform=Mac"])

print("Project build successful!")
