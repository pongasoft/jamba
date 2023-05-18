# Copyright (c) 2020-2023 pongasoft
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

#------------------------------------------------------------------------
# This module delegates to JambaFetchVST3 for determining the location of VST3 SDK
# It then sets up the necessary pieces required by Jamba/VST3
#------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# First we fetch VST3 (optionally)
#-------------------------------------------------------------------------------
include(JambaFetchVST3)

message(STATUS "VST3_SDK_ROOT=${VST3_SDK_ROOT}")
message(STATUS "vst3sdk_SOURCE_DIR=${vst3sdk_SOURCE_DIR}")

#-------------------------------------------------------------------------------
# Options defined prior to including SDK (not changeable for correct Jamba behavior)
#-------------------------------------------------------------------------------
set(SMTG_CREATE_VST2_VERSION OFF CACHE BOOL "Set by Jamba" FORCE)
set(SMTG_ENABLE_TARGET_VARS_LOG OFF CACHE BOOL "Set by Jamba" FORCE) # disable dump variables
set(SMTG_RUN_VST_VALIDATOR OFF CACHE BOOL "Set by Jamba" FORCE) # disable validator (explicit validate step)
set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "Set by Jamba" FORCE) # disable link (explicit install step)
set(SMTG_ADD_VST3_PLUGINS_SAMPLES OFF CACHE BOOL "Set by Jamba" FORCE) # disable plugin samples

#-------------------------------------------------------------------------------
# Including vst3sdk as a subdirectory (requires some variables to be setup)
# call smtg_enable_vst3_sdk() for the plugin
#-------------------------------------------------------------------------------
set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

# For some reason this is needed to remove the visibility warning
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

#-------------------------------------------------------------------------------
# editorhost - excluded by vst3sdk/CMakeLists.txt => need to add it manually
#-------------------------------------------------------------------------------
set(SDK_IDE_HOSTING_EXAMPLES_FOLDER FOLDER "HostingExamples")
set(SMTG_ADD_VST3_HOSTING_SAMPLES ON)
set(SDK_ROOT "${vst3sdk_SOURCE_DIR}")
add_subdirectory(${vst3sdk_SOURCE_DIR}/public.sdk/samples/vst-hosting/editorhost vst3-sdk/editorhost)

#-------------------------------------------------------------------------------
# Fixing too aggressive errors/warnings
#-------------------------------------------------------------------------------
if(CMAKE_HOST_APPLE)
  target_compile_options("base" PRIVATE -w)
  target_compile_options("pluginterfaces" PRIVATE -w)
  target_compile_options("sdk" PRIVATE -w)
  target_compile_options("sdk_common" PRIVATE -w)
  target_compile_options("sdk_hosting" PRIVATE -w)
  target_compile_options("vstgui" PRIVATE -w)
  target_compile_options("vstgui_uidescription" PRIVATE -w)
  target_compile_options("vstgui_support" PRIVATE -w)
  target_compile_options("editorhost" PRIVATE -w)
  target_compile_options("moduleinfotool" PRIVATE -w)
  target_compile_options("validator" PRIVATE -w)
  target_compile_options("VST3Inspector" PRIVATE -w)
endif()
