from ue4helpers import EditorTest
import os

# Get the project root directory from an environment variable
project_root = os.environ.get("GITHUB_WORKSPACE")
if not project_root:
    raise Exception("GITHUB_WORKSPACE environment variable not set")

# Create an EditorTest instance
tester = EditorTest(project_root)

# Run the tests
# You may want to customize the test flags based on your needs
tester.run_tests(test_flags="-ExecCmds="Automation RunTests Project.Functional Tests" -testexit="Automation Test Queue Empty" -ReportExportPath="../Saved/TestReport"")

print("Tests run successfully!")
