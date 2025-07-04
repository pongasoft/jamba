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

#-------------------------------------------------------------------------------
# Platform Detection (for backward compatibility with previous SDK versions which were using this variables)
#-------------------------------------------------------------------------------
if(APPLE)
  set(MAC TRUE)
elseif(WIN32)
  set(WIN TRUE)
endif()

#------------------------------------------------------------------------
# The VST3 SDK version supported by Jamba
#------------------------------------------------------------------------
set(JAMBA_VST3SDK_VERSION "3.7.14" CACHE STRING "VST3 SDK Version (not recommended to change)")

#------------------------------------------------------------------------
# The default location for the VST3 SDK
# Either install the SDK in its default location or provide VST3_SDK_ROOT
#------------------------------------------------------------------------
if(APPLE)
  set(VST3_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.${JAMBA_VST3SDK_VERSION}" CACHE PATH "Location of VST3 SDK")
elseif(WIN32)
  set(VST3_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.${JAMBA_VST3SDK_VERSION}" CACHE PATH "Location of VST3 SDK")
else()
  message(FATAL_ERROR "Jamba currently supports only macOS and Windows")
endif()

#------------------------------------------------------------------------
# Determine behavior when no local install
# Note that the SDK is large so it is preferable to install it locally
# (either in its default location or provide VST3_SDK_ROOT)
#------------------------------------------------------------------------
option(JAMBA_DOWNLOAD_VSTSDK "Download VST SDK if not installed locally" OFF)

#------------------------------------------------------------------------
# The Git repository to fetch the VST3 SDK from
# (only used if JAMBA_DOWNLOAD_VSTSDK is ON and VST3_SDK_ROOT not provided)
#------------------------------------------------------------------------
set(JAMBA_VST3SDK_GIT_REPO "https://github.com/steinbergmedia/vst3sdk" CACHE STRING "Vst3sdk git repository url")

#------------------------------------------------------------------------
# The git tag for fetching the VST3 SDK (should obviously be tied to JAMBA_VST3SDK_VERSION)
# Note that it is using a commit hash and not a tag since per CMake recommendations, it is faster to use
# a hash (local check only) vs tag (requires remote check)
#------------------------------------------------------------------------
set(JAMBA_VST3SDK_GIT_TAG 43b4e366ff84afc9b4247776acbf5e234683b77f CACHE STRING "Vst3sdk git tag")

#------------------------------------------------------------------------
# This (advanced) variable allow for patching the sdk
# (the entire folder will be copied on top of a copy of the sdk)
# With sdk 3.7.8 only on Windows with DLL, does it require patching (win32resourcestream.cpp issue)
#------------------------------------------------------------------------
if(WIN32)
  option(SMTG_CREATE_BUNDLE_FOR_WINDOWS "Create Bundle on Windows for the Plug-ins (New since VST 3.6.10!)" ON)
  if(NOT SMTG_CREATE_BUNDLE_FOR_WINDOWS)
    set(JAMBA_VST3SDK_PATCH_DIR "${JAMBA_ROOT}/vst3sdk_${JAMBA_VST3SDK_VERSION}" CACHE STRING "Vst3sdk patch")
  endif()
endif()

#------------------------------------------------------------------------
# C++ language level (Jamba requires C++17)
#------------------------------------------------------------------------
set(JAMBA_CMAKE_CXX_STANDARD "17" CACHE PATH "C++ version (min 17)")

#------------------------------------------------------------------------
# Option to enable building Audio Unit wrapper (macOS only)
#------------------------------------------------------------------------
option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" OFF)

#------------------------------------------------------------------------
# Option to enable/disable testing (includes GoogleTest)
# Simply set to OFF if you want to use your own testing methodology
# You can also write your own JambaAddTest.cmake module instead.
#------------------------------------------------------------------------
option(JAMBA_ENABLE_TESTING "Enable Testing (GoogleTest)" ON)

#------------------------------------------------------------------------
# Option to enable/disable creating adding a target to create an archive
# Simply set to OFF if you do not want the default archiving mechanism
# You can also write your own JambaCreateArchive.cmake module instead.
#------------------------------------------------------------------------
option(JAMBA_ENABLE_CREATE_ARCHIVE "Enable Creating an archive (zip)" ON)

#------------------------------------------------------------------------
# The git respository to fetch googletest from
#------------------------------------------------------------------------
set(googletest_GIT_REPO "https://github.com/google/googletest" CACHE STRING "googletest git repository URL")

#------------------------------------------------------------------------
# The git tag for googletest
# v1.17.0 (2025/04/33)
#------------------------------------------------------------------------
set(googletest_GIT_TAG "v1.17.0" CACHE STRING "googletest git tag")

#------------------------------------------------------------------------
# The download URL for googletest
#------------------------------------------------------------------------
set(googletest_DOWNLOAD_URL "${googletest_GIT_REPO}/archive/refs/tags/${googletest_GIT_TAG}.zip" CACHE STRING "googletest download url" FORCE)

#------------------------------------------------------------------------
# The download URL hash for googletest
#------------------------------------------------------------------------
set(googletest_DOWNLOAD_URL_HASH "SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff" CACHE STRING "googletest download url hash" FORCE)

#------------------------------------------------------------------------
# Option to enable generating the dev script which allows to build and install the plugin
# with a convenient command line tool.
#------------------------------------------------------------------------
option(JAMBA_ENABLE_DEV_SCRIPT "Enable Dev Script (jamba.py)" ON)

#------------------------------------------------------------------------
# Path to the dev script (allow you to rename it if you don't like the name)
#------------------------------------------------------------------------
if (APPLE)
  set(JAMBA_SCRIPT_COMMAND "${CMAKE_BINARY_DIR}/jamba.sh" CACHE PATH "Path to jamba dev script")
elseif (WIN32)
  set(JAMBA_SCRIPT_COMMAND "${CMAKE_BINARY_DIR}/jamba.bat" CACHE PATH "Path to jamba dev script")
endif ()

#------------------------------------------------------------------------
# Sets some useful variables
#------------------------------------------------------------------------
string(TIMESTAMP JAMBA_CURRENT_YEAR "%Y")
string(TIMESTAMP JAMBA_CURRENT_MONTH "%m")
string(TIMESTAMP JAMBA_CURRENT_DAY "%d")
