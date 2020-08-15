cmake_minimum_required (VERSION 3.17)

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

#------------------------------------------------------------------------
# Jamba Version - use git to fetch exact tag/version
#------------------------------------------------------------------------
set(JAMBA_MAJOR_VERSION 5)
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
if(WIN)
  message(STATUS "Adding compiler options")
  add_compile_options("/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING" "/EHsc" "/D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING" "/D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING")
endif ()

#------------------------------------------------------------------------
# Audio Unit (macOS only) => determine Audio Unit Version
#------------------------------------------------------------------------
if(MAC AND JAMBA_ENABLE_AUDIO_UNIT)
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
# Define main function jamba_add_vst3_plugin
#------------------------------------------------------------------------
include(JambaAddVST3Plugin)

#------------------------------------------------------------------------
# Optionally setup testing
#------------------------------------------------------------------------
if(JAMBA_ENABLE_TESTING)
  include(JambaTesting)
endif()

#------------------------------------------------------------------------
# Behavior moved to jamba_add_vst3_plugin => printing warning messages
#------------------------------------------------------------------------
function(jamba_add_vst3plugin target vst_sources)
  message(WARNING "jamba_add_vst3plugin is no longer supported. Use jamba_add_vst3_plugin instead")
endfunction()

function(jamba_create_archive target plugin_name)
  message(WARNING "jamba_create_archive is no longer supported. Handled by jamba_add_vst3_plugin with RELEASE_FILENAME \"${plugin_name}\" option.")
endfunction()

function(jamba_add_vst3_resource target type filename)
  message(WARNING "jamba_add_vst3_resource is no longer supported. Use UIDESC and RESOURCES arguments when invoking jamba_add_vst3_plugin instead.")
endfunction()

function(jamba_gen_vst3_resources target name)
  message(WARNING "jamba_gen_vst3_resources is no longer supported. Use UIDESC and RESOURCES arguments when invoking jamba_add_vst3_plugin instead.")
endfunction()

function(jamba_dev_scripts target)
  message(WARNING "jamba_dev_scripts is no longer supported. Handled by jamba_add_vst3_plugin.")
endfunction()

