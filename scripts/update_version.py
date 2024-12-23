import os
import re
import subprocess

def get_git_version():
    """
    Retrieves the latest Git tag or falls back to the latest commit hash if no tags exist.
    """
    try:
        # Try to get the latest tag with commit hash if dirty
        version = subprocess.check_output(["git", "describe", "--tags", "--dirty"], stderr=subprocess.DEVNULL).strip().decode("utf-8")
        return version
    except subprocess.CalledProcessError:
        # Fallback to the latest commit hash
        try:
            commit_hash = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], stderr=subprocess.DEVNULL).strip().decode("utf-8")
            return commit_hash
        except subprocess.CalledProcessError:
            raise Exception("Unable to retrieve Git version or commit hash. Ensure this is a Git repository.")

def get_repo_root():
    """
    Retrieves the root directory of the Git repository.
    """
    try:
        repo_root = subprocess.check_output(["git", "rev-parse", "--show-toplevel"], stderr=subprocess.DEVNULL).strip().decode("utf-8")
        return repo_root
    except subprocess.CalledProcessError:
        raise Exception("Unable to determine the repository root. Ensure this is a Git repository.")

def read_header_file(file_path):
    """
    Reads the contents of the header file.
    """
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"File not found: {file_path}")
    with open(file_path, "r") as file:
        return file.read()

def update_header_content(content, git_version):
    """
    Updates the header file content with the new version information.
    """
    # Default version components
    major, minor, patch = "0", "0", "0"

    # Extract major, minor, patch, and optional suffix from Git version
    match = re.match(r"v?(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)(?P<suffix>.*)", git_version)
    if match:
        major = match.group("major")
        minor = match.group("minor")
        patch = match.group("patch")

    # Update macros in the content
    content = re.sub(r'#define\s+UIRB_CORE_LIB_VER_STR\s+".*"', f'#define UIRB_CORE_LIB_VER_STR "{git_version}"', content)
    content = re.sub(r'#define\s+UIRB_CORE_LIB_MAJOR\s+\(\d+\)', f'#define UIRB_CORE_LIB_MAJOR ({major})', content)
    content = re.sub(r'#define\s+UIRB_CORE_LIB_MINOR\s+\(\d+\)', f'#define UIRB_CORE_LIB_MINOR ({minor})', content)
    content = re.sub(r'#define\s+UIRB_CORE_LIB_PATCH\s+\(\d+\)', f'#define UIRB_CORE_LIB_PATCH ({patch})', content)

    return content

def write_header_file(file_path, content):
    """
    Writes the updated content back to the header file.
    """
    with open(file_path, "w") as file:
        file.write(content)

def main():
    try:
        # Determine the repository root
        repo_root = get_repo_root()
        
        # Path to the header file
        header_file_path = os.path.join(repo_root, "include", "UIRBcore_Version.h")

        # Get the latest version from Git
        git_version = get_git_version()
        print(f"Git Tag Version: {git_version}")

        # Read the header file
        content = read_header_file(header_file_path)

        # Update the content with the new version
        updated_content = update_header_content(content, git_version)

        # Write the updated content back to the file
        write_header_file(header_file_path, updated_content)
        print(f"Updated {header_file_path} successfully.")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
