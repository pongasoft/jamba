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
set(SMTG_CREATE_BUNDLE_FOR_WINDOWS OFF CACHE BOOL "Set by Jamba" FORCE) # disable bundle for Windows (can't build vst2 otherwise)

#-------------------------------------------------------------------------------
# Properties for VST3
#-------------------------------------------------------------------------------
set(SDK_ROOT "${VST3_SDK_ROOT}")
set(VST_SDK TRUE)
set_property(GLOBAL PROPERTY SDK_ROOT ${SDK_ROOT})
set(public_sdk_SOURCE_DIR ${SDK_ROOT}/public.sdk)
set(pluginterfaces_SOURCE_DIR ${SDK_ROOT}/pluginterfaces)

#-------------------------------------------------------------------------------
# Includes (not including unsupported options like AAX)
#-------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${VST3_SDK_ROOT}/cmake/modules")

include(SMTG_Global)
include(SMTG_AddVST3Library)
include(SMTG_Bundle)
include(SMTG_ExportedSymbols)
include(SMTG_PrefixHeader)
include(SMTG_PlatformToolset)
include(SMTG_CoreAudioSupport)
include(SMTG_VstGuiSupport)
include(SMTG_UniversalBinary)
include(SMTG_AddVST3Options)

#-------------------------------------------------------------------------------
# Setting up platform toolset
#-------------------------------------------------------------------------------
smtg_setup_platform_toolset()

# the entire SDK is added as include directories
include_directories(${VST3_SDK_ROOT})

#-------------------------------------------------------------------------------
# Adding VSTGUI support (Jamba depends on it)
#-------------------------------------------------------------------------------
set(SMTG_VSTGUI_ROOT "${VST3_SDK_ROOT}")
smtg_enable_vstgui_support()
set_property(GLOBAL PROPERTY SMTG_VSTGUI_ROOT ${SMTG_VSTGUI_ROOT})

#-------------------------------------------------------------------------------
# Setting up Core Audio Support (for macOS)
#-------------------------------------------------------------------------------
setupCoreAudioSupport()

#-------------------------------------------------------------------------------
# Projects
#-------------------------------------------------------------------------------
set(SDK_IDE_LIBS_FOLDER FOLDER "Libraries")

set(SDK_IDE_HOSTING_EXAMPLES_FOLDER FOLDER "HostingExamples")
set(SDK_IDE_PLUGIN_EXAMPLES_FOLDER FOLDER "PlugInExamples")

#-------------------------------------------------------------------------------
# Generate the necessary dependencies
#-------------------------------------------------------------------------------
# 3 main libraries
add_subdirectory(${VST3_SDK_ROOT}/base vst3-sdk/base)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk vst3-sdk/public)
add_subdirectory(${VST3_SDK_ROOT}/pluginterfaces vst3-sdk/pluginterfaces)

# validator & editor
set(SMTG_ADD_VST3_HOSTING_SAMPLES ON)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk/samples/vst-hosting/validator vst3-sdk/validator)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk/samples/vst-hosting/editorhost vst3-sdk/editorhost)

#-------------------------------------------------------------------------------
# IDE sorting
#-------------------------------------------------------------------------------
set_target_properties(sdk PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(base PROPERTIES ${SDK_IDE_LIBS_FOLDER})

set_target_properties(vstgui PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(vstgui_support PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(vstgui_uidescription PROPERTIES ${SDK_IDE_LIBS_FOLDER})

if(TARGET vstgui_standalone)
  set_target_properties(vstgui_standalone PROPERTIES ${SDK_IDE_LIBS_FOLDER})
endif()

