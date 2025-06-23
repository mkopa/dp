#!/bin/bash

# Usage check
if [ "$#" -ne 1 ]; then
    echo "Error: Invalid number of arguments." >&2
    echo "Usage: $0 <path_to_git_directory>" >&2
    exit 1
fi

source_dir="$1"

# Validate path
if [ ! -d "$source_dir" ]; then
    echo "Error: Source directory '$source_dir' does not exist." >&2
    exit 1
fi

if [ ! -d "$source_dir/.git" ]; then
    echo "Error: Directory '$source_dir' does not appear to be a Git repository (no .git directory)." >&2
    exit 1
fi

# Determine portable base64 command
if base64 --version 2>/dev/null | grep -qi 'gnu'; then
    base64_cmd() { base64 "$1"; }
else
    base64_cmd() { base64 -i "$1"; }  # BSD/macOS variant
fi

# Binary file detection (true if non-text)
is_binary() {
    local file_path="$1"
    LC_ALL=C grep -qI . "$file_path"
    return $((! $?))  # invert result: true = binary
}

# Main loop
git -C "$source_dir" ls-files -c --others --exclude-standard | while IFS= read -r file; do
    full_path="${source_dir}/${file}"

    echo " -> Processing: $file" >&2
    echo "--- START FILE: ${file} ---"

    if is_binary "$full_path"; then
        echo "[base64]"
        base64_cmd "$full_path"
    else
        cat "$full_path"
    fi

    echo
    echo "--- END FILE: ${file} ---"
    echo
done

echo "Finished." >&2

