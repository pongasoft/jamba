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

#------------------------------------------------------------------------
# This module delegates to JambaFetchVST3 for determining the location of VST3 SDK
# It then sets up the necessary pieces required by Jamba/VST3
#------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# First we fetch VST3 (optionally)
#-------------------------------------------------------------------------------
include(JambaFetchVST3)

message(STATUS "VST3_SDK_ROOT=${VST3_SDK_ROOT}")

#-------------------------------------------------------------------------------
# Options defined prior to including SDK (not changeable for correct Jamba behavior)
#-------------------------------------------------------------------------------
set(SMTG_CREATE_VST2_VERSION ${JAMBA_ENABLE_VST2} CACHE BOOL "Set by Jamba" FORCE)
set(SMTG_ENABLE_TARGET_VARS_LOG OFF CACHE BOOL "Set by Jamba" FORCE) # disable dump variables
set(SMTG_RUN_VST_VALIDATOR OFF CACHE BOOL "Set by Jamba" FORCE) # disable validator (explicit validate step)
set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "Set by Jamba" FORCE) # disable link (explicit install step)
set(SMTG_ADD_VST3_PLUGINS_SAMPLES OFF CACHE BOOL "Set by Jamba" FORCE) # disable plugin samples
if(WIN32 AND JAMBA_ENABLE_VST2)
    set(SMTG_CREATE_BUNDLE_FOR_WINDOWS OFF CACHE BOOL "Set by Jamba" FORCE) # disable bundle for Windows (can't build vst2 otherwise)
endif()

#-------------------------------------------------------------------------------
# Including vst3sdk as a subdirectory (requires some variables to be setup)
# call smtg_enable_vst3_sdk() for the plugin
# call smtg_setup_symbol_visibility() (which should be done in smtg_enable_vst3_sdk!)
#-------------------------------------------------------------------------------
set(vst3sdk_SOURCE_DIR "${VST3_SDK_ROOT}")
set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")
add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

#-------------------------------------------------------------------------------
# editorhost - excluded by vst3sdk/CMakeLists.txt => need to add it manually
#-------------------------------------------------------------------------------
set(SDK_IDE_HOSTING_EXAMPLES_FOLDER FOLDER "HostingExamples")
set(SMTG_ADD_VST3_HOSTING_SAMPLES ON)
set(SDK_ROOT "${VST3_SDK_ROOT}")
add_subdirectory(${VST3_SDK_ROOT}/public.sdk/samples/vst-hosting/editorhost vst3-sdk/editorhost)
