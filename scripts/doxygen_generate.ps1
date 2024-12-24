<#
.SYNOPSIS
    Script to generate Doxygen documentation with dynamic project version and file version filtering.

.DESCRIPTION
    This script automates the process of running Doxygen with dynamically set environment variables:
    - PROJECT_NUMBER: Retrieved from the Git repository using git_info.ps1.
    - FILE_VERSION_FILTER: Points to git_info.ps1 to retrieve the last commit hash for files.

.PARAMETER Doxyfile
    The Doxygen configuration file to process.

.PARAMETER UpdateVersion
    If specified, updates the `UIRBcore_Version.h` file using `update_version.py`.

.PARAMETER OpenHTML
    If specified, opens the `index.html` file in the `docs/html` directory after successful generation.

.NOTES
    - Ensure this script and git_info.ps1 are in the `scripts` directory of your project.
    - Requires Git, Python, and Doxygen to be installed and accessible via the command line.
    - Assumes git_info.ps1 and update_version.py are configured correctly.

.EXAMPLE
    .\doxygen_generate.ps1 .\Doxyfile -UpdateVersion -OpenHTML
#>

param (
    [Parameter(Mandatory = $true)]
    [string]$Doxyfile,

    [switch]$UpdateVersion,

    [switch]$OpenHTML
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

# Update the UIRBcore_Version.h if the -UpdateVersion flag is specified
if ($UpdateVersion) {
    Write-Output "Updating UIRBcore_Version.h..."
    & python "$ScriptDir\update_version.py"
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Error: Updating UIRBcore_Version.h failed."
        exit $LASTEXITCODE
    }
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

Write-Output "Doxygen documentation generated successfully."

# Open index.html if the -OpenHTML flag is specified
if ($OpenHTML) {
    $HtmlPath = "$ScriptDir\..\docs\html\index.html"
    if (Test-Path $HtmlPath) {
        Write-Output "Opening index.html in docs/html directory..."
        Start-Process -FilePath $HtmlPath
    } else {
        Write-Error "Error: index.html not found at $HtmlPath."
    }
}
