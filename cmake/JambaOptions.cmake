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
set(JAMBA_VST3SDK_VERSION "3.7.1" CACHE STRING "VST3 SDK Version (not recommended to change)")

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
set(JAMBA_VST3SDK_GIT_TAG 79750cb3e56f3988f33bc178884962cc3f4c3eb9 CACHE STRING "Vst3sdk git tag")

#------------------------------------------------------------------------
# C++ language level (Jamba requires C++17)
#------------------------------------------------------------------------
set(JAMBA_CMAKE_CXX_STANDARD "17" CACHE PATH "C++ version (min 17)")

#------------------------------------------------------------------------
# Option to enable building VST2 wrapper
#------------------------------------------------------------------------
option(JAMBA_ENABLE_VST2 "Use VST2" OFF)

#------------------------------------------------------------------------
# The VST2 SDK version supported by Jamba
#------------------------------------------------------------------------
set(JAMBA_VST2SDK_VERSION "2.4" CACHE STRING "VST2 SDK Version (not recommended to change... no new version!)")

#------------------------------------------------------------------------
# Determine local / default location
# Either install the SDK in its default location or provide VST2_SDK_ROOT
#------------------------------------------------------------------------
if(APPLE)
  set(VST2_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.${JAMBA_VST2SDK_VERSION}" CACHE PATH "Location of VST2 SDK")
elseif(WIN32)
  set(VST2_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.${JAMBA_VST2SDK_VERSION}" CACHE PATH "Location of VST2 SDK")
endif()

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
# release-1.10.0 => 703bd9caab50b139428cea1aaff9974ebee5742e
#------------------------------------------------------------------------
set(googletest_GIT_TAG "703bd9caab50b139428cea1aaff9974ebee5742e" CACHE STRING "googletest git tag")

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
