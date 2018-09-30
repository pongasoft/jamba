@Echo OFF
set SRC_DIR=%~dp0
set BUILD_DIR=%CD%\build

Echo BUILD_DIR=%BUILD_DIR%
Echo SRC_DIR=%SRC_DIR%

set CONFIG=Debug
IF not [%1]==[] set CONFIG=%1

mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake -G"Visual Studio 15 2017 Win64" -DJAMBA_DEFAULT_CONFIG=%CONFIG% %SRC_DIR%
