cmake_minimum_required(VERSION 3.19)

# build VST 2?
option(JAMBA_ENABLE_VST2 "Use VST2" ON)

# build Audio Unit?
option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" ON)

# Sets the deployment target for macOS
set(JAMBA_MACOS_DEPLOYMENT_TARGET "10.14" CACHE STRING "macOS deployment target")

# To use local jamba install, uncomment the following line (no download)
set(JAMBA_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

# Determine proper architecture for the project
include("${JAMBA_ROOT_DIR}/cmake/JambaSetArchitecture.cmake")

# can't set version because it must match jamba version (chicken and egg problem)
set(JAMBA_PROJECT_VERSION_WARNING_DISABLED ON)

project(JambaTestPlugin)

# To use local googletest install, uncomment the following line (no download) and modify the path accordingly
unset(GOOGLETEST_ROOT_DIR)
#set(GOOGLETEST_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../google/googletest")

# finally we include jamba itself
include(${JAMBA_ROOT_DIR}/jamba.cmake)

set(PLUGIN_MAJOR_VERSION ${JAMBA_MAJOR_VERSION})
set(PLUGIN_MINOR_VERSION ${JAMBA_MINOR_VERSION})
set(PLUGIN_PATCH_VERSION ${JAMBA_PATCH_VERSION})
set(PLUGIN_VERSION "${PLUGIN_MAJOR_VERSION}.${PLUGIN_MINOR_VERSION}.${PLUGIN_PATCH_VERSION}")

set(CPP_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/cpp")

# We need to recompute the Audio Unit version (AU_PLUGIN_VERSION_HEX) because PLUGIN_MAJOR_VERSION (...) are set
# AFTER including jamba for this plugin only
execute_process(COMMAND bash -c "echo 'obase=16;${PLUGIN_MAJOR_VERSION}*65536+${PLUGIN_MINOR_VERSION}*256+${PLUGIN_PATCH_VERSION}' | bc"
    OUTPUT_VARIABLE AU_PLUGIN_VERSION_HEX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

# Generate the version.h file
set(VERSION_DIR "${CMAKE_BINARY_DIR}/generated")
configure_file("${CPP_SOURCES}/version.h.in" "${VERSION_DIR}/version.h")

if (JAMBA_ENABLE_VST2)
  set(vst2_sources
      "${CPP_SOURCES}/JambaTestPlugin_VST2.cpp"
      )
endif ()

set(vst_sources
    "${CPP_SOURCES}/JambaTestPlugin_VST3.cpp"

    "${CPP_SOURCES}/JambaTestPluginCIDs.h"
    "${CPP_SOURCES}/Model.h"
    "${CPP_SOURCES}/Plugin.h"
    "${CPP_SOURCES}/Plugin.cpp"

    "${CPP_SOURCES}/RT/JambaTestPluginProcessor.h"
    "${CPP_SOURCES}/RT/JambaTestPluginProcessor.cpp"

    "${CPP_SOURCES}/GUI/JambaTestPluginController.h"
    "${CPP_SOURCES}/GUI/JambaTestPluginController.cpp"
    )

# Defining the list of resources
set(RES_DIR "${CMAKE_CURRENT_LIST_DIR}/resource")

set(vst_resources
    "${RES_DIR}/button_2frames.png"
    "${RES_DIR}/button_3frames.png"
    "${RES_DIR}/button_4frames.png"
    "${RES_DIR}/image_2frames.png"
    "${RES_DIR}/image_3frames.png"
    "${RES_DIR}/image_4frames.png"
    )

#------------------------------------------------------------------------
# Testing - for jamba (NOT the plugin)
#------------------------------------------------------------------------
set(JAMBA_TEST_CASES_DIR "${JAMBA_ROOT}/test/cpp")
set(JAMBA_TEST_CASES_SOURCES
    "${JAMBA_TEST_CASES_DIR}/pongasoft/Utils/Collection/test-CircularBuffer.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/Utils/Concurrent/test-concurrent.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/Utils/Concurrent/test-concurrent_lockfree.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/Utils/test-Lerp.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/Utils/test-StringUtils.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/GUI/Params/test-GUIParameters.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/GUI/Params/test-ParamAware.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/GUI/Views/test-CustomViewCreator.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/GUI/Views/test-SelfContainedViewListener.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/test-AudioBuffers.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/test-AudioUtils.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/test-ParamConverters.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/test-SampleRateBasedClock.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/Utils/test-Utils.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/Utils/test-FastWriteMemoryStream.cpp"
    "${JAMBA_TEST_CASES_DIR}/pongasoft/VST/Utils/test-ReadOnlyMemoryStream.cpp"
    )

jamba_add_vst_plugin(
    TARGET               "pongasoft_JambaTestPlugin" # name of CMake target for the plugin
    RELEASE_FILENAME     "JambaTestPlugin" # filename for the plugin (xxx.vst3)
    ARCHIVE_ARCHITECTURE "${JAMBA_ARCHIVE_ARCHITECTURE}"
    TARGETS_PREFIX       "jmb_" # prefix used by all targets directly linked to this plugin
    VST_SOURCES          "${vst_sources}" "${vst2_sources}" # the source files that compose the plugin
    INCLUDE_DIRECTORIES  "${VERSION_DIR}" # we add the version folder to the list of includes
    UIDESC               "${RES_DIR}/JambaTestPlugin.uidesc" # the main xml file for the GUI
    RESOURCES            "${vst_resources}" # the resources for the GUI (png files)
    TEST_CASE_SOURCES    "${JAMBA_TEST_CASES_SOURCES}" # the source files containing the test cases
    TEST_LINK_LIBRARIES  "jamba" # the library needed for linking the tests
)
