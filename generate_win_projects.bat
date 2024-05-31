@echo off
echo Choose the target environment for project generation:
echo 1. Visual Studio 2017
echo 2. Visual Studio 2019
echo 3. Visual Studio 2022
set /p choice="Enter your choice (1-3): "

if "%choice%"=="1" (
    echo Generating project files for Visual Studio 2017...
    call vendor\premake\bin\premake5.exe vs2017
) else if "%choice%"=="2" (
    echo Generating project files for Visual Studio 2019...
    call vendor\premake\bin\premake5.exe vs2019
) else if "%choice%"=="3" (
    echo Generating project files for Visual Studio 2022...
    call vendor\premake\bin\premake5.exe vs2022
) else (
    echo Invalid choice, exiting.
)

popd
PAUSE