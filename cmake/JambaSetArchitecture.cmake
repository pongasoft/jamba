# Copyright (c) 2020 pongasoft
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# @author Yan Pujante

########################################################
# This file must be included PRIOR to invoking project()
########################################################

# Enable/Disable universal build on Apple Silicon
option(JAMBA_ENABLE_XCODE_UNIVERSAL_BUILD "Enable building x86/arm64 universal binary on Apple Silicon" ON)

# Deployment target/architectures for macOS
if(APPLE)
  # set the deployment target if provided
  if(JAMBA_MACOS_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "${JAMBA_MACOS_DEPLOYMENT_TARGET}" CACHE STRING "" FORCE)
  endif()

  # on macOS "uname -m" returns the architecture (x86_64 or arm64)
  execute_process(
      COMMAND uname -m
      RESULT_VARIABLE result
      OUTPUT_VARIABLE JAMBA_OSX_NATIVE_ARCHITECTURE
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # determine if we do universal build or native build
  if(JAMBA_ENABLE_XCODE_UNIVERSAL_BUILD                     # is universal build enabled?
      AND (CMAKE_GENERATOR STREQUAL "Xcode")                # works only with Xcode
      AND (JAMBA_OSX_NATIVE_ARCHITECTURE STREQUAL "arm64")) # and only when running on arm64
    option(SMTG_BUILD_UNIVERSAL_BINARY "Build universal binary (x86_64 & arm64)" ON)
    set(JAMBA_ARCHIVE_ARCHITECTURE "macOS_universal")
    message(STATUS "macOS universal (x86_64 / arm64) build")
  else()
    option(SMTG_BUILD_UNIVERSAL_BINARY "Build universal binary (x86_64 & arm64)" OFF)
    set(JAMBA_ARCHIVE_ARCHITECTURE "macOS_${JAMBA_OSX_NATIVE_ARCHITECTURE}")
    message(STATUS "macOS native ${JAMBA_OSX_NATIVE_ARCHITECTURE} build")
  endif()

elseif(WIN32)
  # nothing special for windows
  set(JAMBA_ARCHIVE_ARCHITECTURE "win_64bits")
  message(STATUS "Windows native build")
endif()
