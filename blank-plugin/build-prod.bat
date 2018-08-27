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

Echo =============================================================
Echo ==                                                         ==
Echo ==              Running Tests..........                    ==
Echo ==              -----------------------                    ==
Echo =============================================================
%SRC_DIR%\test.bat Release

Echo =============================================================
Echo ==                                                         ==
Echo ==              Validating Plugin......                    ==
Echo ==              -----------------------                    ==
Echo =============================================================
%SRC_DIR%\validate.bat Release

Echo =============================================================
Echo ==                                                         ==
Echo ==              Building archive.......                    ==
Echo ==              -----------------------                    ==
Echo =============================================================
cmake --build . --config Release --target archive
