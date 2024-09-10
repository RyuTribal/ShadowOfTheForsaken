#!/bin/bash
if command -v git &> /dev/null
then
    echo "git found. Checking for modified files..."

    MODIFIED_FILES=$(git diff --name-only HEAD -- src/ | grep -E '\.(cpp|h)$')


    if [ -n "$MODIFIED_FILES" ]; then
        echo "Formatting modified files..."
        echo "$MODIFIED_FILES" | xargs clang-format -i
    else
        echo "No modified files to format."
    fi
else
    echo "git not found. Running clang-format on all files."

    find src -name '*.cpp' -o -name '*.h' | xargs clang-format -i
fi

