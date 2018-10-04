@Echo OFF
set SRC_DIR=%~dp0
set BUILD_DIR=%CD%\build

Echo BUILD_DIR=%BUILD_DIR%
Echo SRC_DIR=%SRC_DIR%

mkdir %BUILD_DIR%
cd %BUILD_DIR%

Echo =============================================================
Echo ==                                                         ==
Echo ==              Generating Makefiles...                    ==
Echo ==              -----------------------                    ==
Echo =============================================================
cmake -G"Visual Studio 15 2017 Win64" %SRC_DIR%

%SRC_DIR%\jamba.bat prod
