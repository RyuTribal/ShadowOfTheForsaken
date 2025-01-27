#!/bin/bash
vendor/premake/premake5 gmake2

bear -- make 

PROJECT_ROOT="$(dirname "$0")"

# Create .vscode directory if it doesn't exist
mkdir -p "$PROJECT_ROOT/.vscode"

# Create tasks.json for building the project
cat > "$PROJECT_ROOT/.vscode/tasks.json" << EOF
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
EOF

# Create launch.json for debugging
cat > "$PROJECT_ROOT/.vscode/launch.json" << EOF
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/Game/bin/Debug-linux-x86_64/Game/Game",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}/Game",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
EOF

cat > $PROJECT_ROOT/.vscode/c_cpp_properties.json << EOF
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "\${workspaceFolder}/**"
            ],
            "defines": [
                "DEBUG",
                "UNICODE"
            ],
            "compilerPath": "/usr/bin/gcc", 
            "cStandard": "c11",
            "cppStandard": "c++20",
            "intelliSenseMode": "\${default}",
            "compileCommands": "${workspaceFolder}/compile_commands.json"
        }
    ],
    "version": 4
}
EOF

cat > "$PROJECT_ROOT/.vscode/settings.json" << EOF
{
    "C_Cpp.clang_format_path": "/usr/bin/clang-format",
    "C_Cpp.clang_format_style": "file",
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "[cpp]": {
        "editor.defaultFormatter": "ms-vscode.cpptools"
    },
    "[c]": {
        "editor.defaultFormatter": "ms-vscode.cpptools"
    }
}
EOF

echo "VS Code configuration files have been created/updated."
