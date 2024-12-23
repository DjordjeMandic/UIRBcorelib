<#
.SYNOPSIS
    Script to retrieve Git project version or file-specific commit information for Doxygen.

.DESCRIPTION
    This script provides two functionalities:
    1. Retrieve the project version based on the latest Git tag or commit hash.
    2. Retrieve the last commit hash for a specified file.

.PARAMETER Operation
    The operation to perform. Valid options are:
    - "project_number": Outputs the project version.
    - "file_version": Outputs the last commit hash for a specified file.

.PARAMETER File
    The file for which to retrieve the last commit hash (only used for "file_version").

.NOTES
    - Ensure this script is executed in a Git repository.
    - Requires Git to be installed and accessible via the command line.

.EXAMPLES
    .\git_info.ps1 project_number
    .\git_info.ps1 file_version <file>
#>

param (
    [Parameter(Mandatory = $true)]
    [ValidateSet("project_number", "file_version")]
    [string]$Operation,

    [Parameter(Mandatory = $false)]
    [string]$File
)

# Perform the requested operation
switch ($Operation) {
    "project_number" {
        <#
        .SYNOPSIS
            Retrieve the project version.

        .DESCRIPTION
            Uses `git describe` to get the latest tag and commit description.
            Falls back to the short commit hash if no tags are found.

        .OUTPUTS
            The project version string or commit hash.
        #>
        $projVer = git describe --tags --dirty 2>$null
        if (-not $projVer) {
            $projVer = git rev-parse --short HEAD
        }
        Write-Output $projVer
    }
    "file_version" {
        <#
        .SYNOPSIS
            Retrieve the last commit hash for a specified file.

        .PARAMETER File
            The file for which to retrieve the last commit hash.

        .OUTPUTS
            The commit hash for the last modification of the file.

        .NOTES
            Outputs "error" if no file is specified.
        #>
        if (-not $File) {
            Write-Output "error"
            exit 1
        }
        $commitHash = git log -n 1 --pretty=format:"%h (%ad)" -- $File
        Write-Output $commitHash
    }
    default {
        <#
        .SYNOPSIS
            Display usage information.

        .OUTPUTS
            Usage message.
        #>
        Write-Output "Usage: .\git_info.ps1 {project_number|file_version} [file]"
        exit 1
    }
}
