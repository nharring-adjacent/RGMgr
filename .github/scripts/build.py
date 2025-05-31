from ue4helpers import ProjectBuild
import os

# Get the project root directory from an environment variable
project_root = os.environ.get("GITHUB_WORKSPACE")
if not project_root:
    raise Exception("GITHUB_WORKSPACE environment variable not set")

# Create a ProjectBuild instance
builder = ProjectBuild(project_root)

# Build the project for Win64, Linux, and Mac
# You may want to customize these platforms based on your needs
builder.build("Win64", "Development")
builder.build("Linux", "Development")
builder.build("Mac", "Development")

print("Project build successful!")
