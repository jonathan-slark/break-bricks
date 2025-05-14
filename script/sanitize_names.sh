#!/usr/bin/env bash

# Recursively sanitize file and directory names

sanitize() {
    local original="$1"
    # Replace problematic characters with underscores or remove them
    local sanitized
    sanitized=$(echo "$original" | sed -E "s/[[:space:]]+/_/g; s/['\"()&!?,;]//g")

    if [[ "$original" != "$sanitized" ]]; then
        mv -v -- "$original" "$sanitized"
    fi
}

export -f sanitize

# Rename files and directories bottom-up to avoid path conflicts
find . -depth -name "*[ '\"()&!?,;]*" -exec bash -c 'sanitize "$0"' {} \;
