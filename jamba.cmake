# Copyright (c) 2020-2021 pongasoft
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

cmake_minimum_required (VERSION 3.19)

#------------------------------------------------------------------------
# Defining JAMBA_ROOT
#------------------------------------------------------------------------
set(JAMBA_ROOT ${CMAKE_CURRENT_LIST_DIR})

#------------------------------------------------------------------------
# Adding cmake folder to cmake path => allow for Jamba cmake files +
# override PlatformToolset
#------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${JAMBA_ROOT}/cmake")

#------------------------------------------------------------------------
# Including files containing the various options for Jamba (in one place)
#------------------------------------------------------------------------
include(JambaOptions)

# This needs to be done as soon as possible
if(JAMBA_ENABLE_TESTING)
  enable_testing()
endif()

#------------------------------------------------------------------------
# Jamba Version - use git to fetch exact tag/version
#------------------------------------------------------------------------
set(JAMBA_MAJOR_VERSION 6)
set(JAMBA_MINOR_VERSION 0)
set(JAMBA_PATCH_VERSION 0)
execute_process(COMMAND git describe --long --dirty --abbrev=10 --tags
    RESULT_VARIABLE result
    OUTPUT_VARIABLE JAMBA_GIT_VERSION
    WORKING_DIRECTORY ${JAMBA_ROOT}
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git describe --tags
    RESULT_VARIABLE result
    OUTPUT_VARIABLE JAMBA_GIT_TAG
    WORKING_DIRECTORY ${JAMBA_ROOT}
    OUTPUT_STRIP_TRAILING_WHITESPACE)
set(JAMBA_VERSION "${JAMBA_MAJOR_VERSION}.${JAMBA_MINOR_VERSION}.${JAMBA_PATCH_VERSION}")
message(STATUS "jamba git version - ${JAMBA_GIT_VERSION} | jamba git tag - ${JAMBA_GIT_TAG}")


#------------------------------------------------------------------------
# Setting local property for multi config (ex: XCode) vs single config (ex: Makefile)
#------------------------------------------------------------------------
get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

#------------------------------------------------------------------------
# Compiler options (general)
#------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD ${JAMBA_CMAKE_CXX_STANDARD})
if(WIN32)
  message(STATUS "Adding compiler options")
  add_compile_options("/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING" "/EHsc" "/D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING" "/D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING")
endif ()

#------------------------------------------------------------------------
# Audio Unit (macOS only) => determine Audio Unit Version
#------------------------------------------------------------------------
if(APPLE AND JAMBA_ENABLE_AUDIO_UNIT)
  if(NOT SMTG_COREAUDIO_SDK_PATH)
    set(SMTG_COREAUDIO_SDK_PATH "${JAMBA_ROOT}/audio-unit/CoreAudioSDK/CoreAudio")
  endif()
  # Computes the Audio Unit version (AU_PLUGIN_VERSION_HEX)
  execute_process(COMMAND bash -c "echo 'obase=16;${PLUGIN_MAJOR_VERSION}*65536+${PLUGIN_MINOR_VERSION}*256+${PLUGIN_PATCH_VERSION}' | bc"
      OUTPUT_VARIABLE AU_PLUGIN_VERSION_HEX
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
endif ()


#------------------------------------------------------------------------
# Including VST3 SDK
#------------------------------------------------------------------------
include(JambaSetupVST3)

# Defining VST3_OUTPUT_DIR which is the location where the VST3 artifact is hosted
set(VST3_OUTPUT_DIR ${CMAKE_BINARY_DIR}/VST3)

if(WIN32)
  # Patching win32resourcestream.cpp which contains a bug
  get_target_property(vstgui_sources vstgui SOURCES)
  list(REMOVE_ITEM vstgui_sources "platform/win32/win32resourcestream.cpp")
  list(APPEND vstgui_sources "${CMAKE_CURRENT_LIST_DIR}/src/cpp/vstgui4/vstgui/lib/platform/win32/win32resourcestream.cpp")
  set_target_properties(vstgui PROPERTIES SOURCES "${vstgui_sources}")
  target_include_directories(vstgui PRIVATE "${SMTG_VSTGUI_ROOT}")
endif()

#------------------------------------------------------------------------
# Optionally including VST2 SDK
#------------------------------------------------------------------------
if(JAMBA_ENABLE_VST2)
  include(JambaSetupVST2)
endif ()

#------------------------------------------------------------------------
# Add jamba source code => generate jamba library
#------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src jamba)

#------------------------------------------------------------------------
# Define main function jamba_add_vst_plugin
#------------------------------------------------------------------------
include(JambaAddVSTPlugin)

#------------------------------------------------------------------------
# Behavior moved to jamba_add_vst_plugin => printing deprecation messages
#------------------------------------------------------------------------
function(jamba_add_vst3plugin)
  message(DEPRECATION "Since 5.0.0 - jamba_add_vst3plugin is no longer supported. Use jamba_add_vst_plugin instead")
endfunction()

function(jamba_create_archive)
  message(DEPRECATION "Since 5.0.0 - jamba_create_archive is no longer supported. Handled by jamba_add_vst_plugin with RELEASE_FILENAME \"${plugin_name}\" option.")
endfunction()

function(jamba_add_vst3_resource)
  message(DEPRECATION "Since 5.0.0 - jamba_add_vst3_resource is no longer supported. Use UIDESC and RESOURCES arguments when invoking jamba_add_vst_plugin instead.")
endfunction()

function(jamba_gen_vst3_resources)
  message(DEPRECATION "Since 5.0.0 - jamba_gen_vst3_resources is no longer supported. Use UIDESC and RESOURCES arguments when invoking jamba_add_vst_plugin instead.")
endfunction()

function(jamba_dev_scripts)
  message(DEPRECATION "Since 5.0.0 - jamba_dev_scripts is no longer supported. Handled by jamba_add_vst_plugin.")
endfunction()

function(jamba_add_test)
  message(DEPRECATION "Since 5.0.0 - jamba_add_test is no longer supported. Handled by jamba_add_vst_plugin.")
endfunction()
