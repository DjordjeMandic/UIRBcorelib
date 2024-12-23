<#
.SYNOPSIS
    Script to generate Doxygen documentation with dynamic project version and file version filtering.

.DESCRIPTION
    This script automates the process of running Doxygen with dynamically set environment variables:
    - PROJECT_NUMBER: Retrieved from the Git repository using git_info.ps1.
    - FILE_VERSION_FILTER: Points to git_info.ps1 to retrieve the last commit hash for files.

.PARAMETER Doxyfile
    The Doxygen configuration file to process.

.NOTES
    - Ensure this script and git_info.ps1 are in the `scripts` directory of your project.
    - Requires Git and Doxygen to be installed and accessible via the command line.
    - Assumes git_info.ps1 is configured correctly to retrieve project and file versions.

.EXAMPLE
    .\doxygen_generate.ps1 .\Doxyfile
#>

param (
    [Parameter(Mandatory = $true)]
    [string]$Doxyfile
)

# Ensure the specified Doxyfile exists
if (-not (Test-Path $Doxyfile)) {
    Write-Error "Error: Specified Doxyfile '$Doxyfile' not found."
    exit 1
}

# Get the script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition

# Retrieve the project number using git_info.ps1
$ProjectNumber = & "$ScriptDir\git_info.ps1" project_number

if (-not $ProjectNumber) {
    Write-Error "Error: Unable to retrieve project number from git_info.ps1."
    exit 1
}

# Set the environment variables
$env:PROJECT_NUMBER = $ProjectNumber

# TODO: make this command faster
$env:FILE_VERSION_FILTER = "pwsh -NoProfile -File `"$ScriptDir\git_info.ps1`" file_version"

# Run Doxygen
Write-Output "Running Doxygen with PROJECT_NUMBER=$env:PROJECT_NUMBER and FILE_VERSION_FILTER=$env:FILE_VERSION_FILTER..."
& doxygen $Doxyfile

if ($LASTEXITCODE -ne 0) {
    Write-Error "Error: Doxygen execution failed."
    exit $LASTEXITCODE
}
