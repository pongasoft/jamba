#-------------------------------------------------------------------------------
# Platform Detection
#-------------------------------------------------------------------------------

if(APPLE)
  set(MAC TRUE)
elseif(WIN32)
  set(WIN TRUE)
elseif(UNIX)
  set(LINUX TRUE)
endif()

if(MAC)
  set(VST3_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.369/VST3_SDK" CACHE PATH "Location of VST3 SDK")
elseif(WIN)
  set(VST3_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.369/VST3_SDK" CACHE PATH "Location of VST3 SDK")
else()
  set(VST3_SDK_ROOT "" CACHE PATH "Location of VST3 SDK")
endif()

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

if(VST3_SDK_ROOT)
  MESSAGE(STATUS "VST3_SDK_ROOT=${VST3_SDK_ROOT}")
else()
  MESSAGE(FATAL_ERROR "VST3_SDK_ROOT is not defined. Please use -DVST3_SDK_ROOT=<path to VST3 sdk>.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${VST3_SDK_ROOT}/cmake/modules")

include(Global)
include(AddVST3Library)
include(Bundle)
include(ExportedSymbols)
include(PrefixHeader)
include(PlatformIOS)
include(PlatformToolset)
include(VstGuiSupport)
include(UniversalBinary)
include(AddVST3Options)

#-------------------------------------------------------------------------------
# SDK Project
#-------------------------------------------------------------------------------
setupPlatformToolset()

# Here you can define where the VST 3 SDK is located
set(SDK_ROOT "${VST3_SDK_ROOT}")
set_property(GLOBAL PROPERTY SDK_ROOT ${SDK_ROOT})

# Here you can define where the VSTGUI is located
if(SMTG_ADD_VSTGUI)
  set(VSTGUI_ROOT "${VST3_SDK_ROOT}")
  setupVstGuiSupport()
  set_property(GLOBAL PROPERTY VSTGUI_ROOT ${VSTGUI_ROOT})
endif()

include_directories(${SDK_ROOT})

#-------------------------------------------------------------------------------
# Projects
#-------------------------------------------------------------------------------
set(SDK_IDE_LIBS_FOLDER FOLDER "Libraries")

set(SDK_IDE_HOSTING_EXAMPLES_FOLDER FOLDER "HostingExamples")
set(SDK_IDE_PLUGIN_EXAMPLES_FOLDER FOLDER "PlugInExamples")

#---Add base libraries---------------------------
add_subdirectory(${VST3_SDK_ROOT}/base vst3-sdk/base)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk vst3-sdk/public)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk/source/vst/interappaudio vst3-sdk/interappaudio)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk/samples/vst-hosting/validator vst3-sdk/validator)
add_subdirectory(${VST3_SDK_ROOT}/public.sdk/samples/vst-hosting/editorhost vst3-sdk/editorhost)

#-------------------------------------------------------------------------------
# IDE sorting
#-------------------------------------------------------------------------------
set_target_properties(sdk PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(base PROPERTIES ${SDK_IDE_LIBS_FOLDER})

if(SMTG_ADD_VSTGUI)
  set_target_properties(vstgui PROPERTIES ${SDK_IDE_LIBS_FOLDER})
  set_target_properties(vstgui_support PROPERTIES ${SDK_IDE_LIBS_FOLDER})
  set_target_properties(vstgui_uidescription PROPERTIES ${SDK_IDE_LIBS_FOLDER})

  if(TARGET vstgui_standalone)
    set_target_properties(vstgui_standalone PROPERTIES ${SDK_IDE_LIBS_FOLDER})
  endif()
endif()

if(MAC AND XCODE AND SMTG_IOS_DEVELOPMENT_TEAM)
  set_target_properties(base_ios PROPERTIES ${SDK_IDE_LIBS_FOLDER})
endif()
