
#-------------------------------------------------------------------------------
# Handles default VST3_SDK_ROOT location if not defined
#-------------------------------------------------------------------------------

if(NOT VST3_SDK_ROOT)
  if(MAC)
    set(VST3_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.370" CACHE PATH "Location of VST3 SDK")
  elseif(WIN)
    set(VST3_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.370" CACHE PATH "Location of VST3 SDK")
  else()
    message(FATAL_ERROR "VST3_SDK_ROOT is not defined. Please use -DVST3_SDK_ROOT=<path to VST3 sdk>.")
  endif()
endif()

#-------------------------------------------------------------------------------
# Check for valid VST3 location
#-------------------------------------------------------------------------------

# This is the file that contains the version number... so should exist!
set(VSTSDK3_KNOWN_FILE "pluginterfaces/vst/vsttypes.h")

if(NOT EXISTS "${VST3_SDK_ROOT}/${VSTSDK3_KNOWN_FILE}")
  if(EXISTS "${VST3_SDK_ROOT}/VST3_SDK/${VSTSDK3_KNOWN_FILE}")
    set(VST3_SDK_ROOT "${VST3_SDK_ROOT}/VST3_SDK" CACHE PATH "Location of VST3 SDK" FORCE)
    message(WARNING "VST3_SDK_ROOT should point to the SDK directly and has been adjusted to ${VST3_SDK_ROOT}")
  else()
    message(FATAL_ERROR "VST3_SDK_ROOT=${VST3_SDK_ROOT} does not seem to point to a valid VST3 SDK")
  endif()
endif()

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

