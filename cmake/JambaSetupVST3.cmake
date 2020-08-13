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
# Options defined prior to including SDK
#-------------------------------------------------------------------------------
option(SMTG_CREATE_VST2_VERSION "" ${JAMBA_ENABLE_VST2})
option(SMTG_ENABLE_TARGET_VARS_LOG "" OFF) # disable dump variables
option(SMTG_RUN_VST_VALIDATOR "" OFF) # disable validator (explicit validate step)
option(SMTG_CREATE_PLUGIN_LINK "" OFF) # disable link (explicit install step)
option(SMTG_ADD_VST3_PLUGINS_SAMPLES "" OFF) # disable plugin samples

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

include(Global)
include(AddVST3Library)
include(Bundle)
include(ExportedSymbols)
include(PrefixHeader)
include(PlatformToolset)
include(CoreAudioSupport)
include(VstGuiSupport)
include(UniversalBinary)
include(AddVST3Options)

#-------------------------------------------------------------------------------
# Setting up platform toolset
#-------------------------------------------------------------------------------
setupPlatformToolset()

# the entire SDK is added as include directories
include_directories(${VST3_SDK_ROOT})

#-------------------------------------------------------------------------------
# Adding VSTGUI support (Jamba depends on it)
#-------------------------------------------------------------------------------
set(VSTGUI_ROOT "${VST3_SDK_ROOT}")
setupVstGuiSupport()
set_property(GLOBAL PROPERTY VSTGUI_ROOT ${VSTGUI_ROOT})

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

