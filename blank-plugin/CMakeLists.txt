cmake_minimum_required(VERSION 3.19)

# build Audio Unit?
option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" [-enable_audio_unit-])

# Download VST SDK if not installed?
option(JAMBA_DOWNLOAD_VSTSDK "Download VST SDK if not installed" [-download_vst_sdk-])

# Sets the deployment target for macOS
set(JAMBA_MACOS_DEPLOYMENT_TARGET "[-macos_deployment_target-]" CACHE STRING "macOS deployment target")

# To use local jamba install, uncomment the following line (no download)
[-remote_jamba-]set(JAMBA_ROOT_DIR "")
[-local_jamba-]set(JAMBA_ROOT_DIR "[-jamba_root_dir-]")

# download jamba framework
include(fetch_jamba.cmake)

# Determine proper architecture for the project
include("${JAMBA_ROOT_DIR}/cmake/JambaSetArchitecture.cmake")

# Plugin version
set(PLUGIN_MAJOR_VERSION 1)
set(PLUGIN_MINOR_VERSION 0)
set(PLUGIN_PATCH_VERSION 0)
set(PLUGIN_VERSION "${PLUGIN_MAJOR_VERSION}.${PLUGIN_MINOR_VERSION}.${PLUGIN_PATCH_VERSION}")

# Project
project("[-name-]" VERSION "${PLUGIN_VERSION}")

# To use local googletest install, uncomment the following line (no download) and modify the path accordingly
set(GOOGLETEST_ROOT_DIR "")
#set(GOOGLETEST_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../google/googletest")

# Include Jamba
include("${JAMBA_ROOT_DIR}/jamba.cmake")

set(CPP_SOURCES "src/cpp")

# Generating the version.h header file which contains the plugin version (to make sure it is in sync with the version
# defined here)
set(VERSION_DIR "${CMAKE_BINARY_DIR}/generated")
configure_file("${CPP_SOURCES}/version.h.in" "${VERSION_DIR}/version.h")

# Source files for VST3
set(vst_sources
    "${CPP_SOURCES}/[-name-]_VST3.cpp"

    "${CPP_SOURCES}/Plugin.h"
    "${CPP_SOURCES}/[-name-]CIDs.h"

    "${CPP_SOURCES}/RT/[-name-]Processor.h"
    "${CPP_SOURCES}/RT/[-name-]Processor.cpp"

    "${CPP_SOURCES}/GUI/[-name-]Controller.h"
    "${CPP_SOURCES}/GUI/[-name-]Controller.cpp"
    )

# Location of resources
set(RES_DIR "${CMAKE_CURRENT_LIST_DIR}/resource")

# List of resources (images)
set(vst_resources
#    "${RES_DIR}/background.png"
)

# Location of the test cases
set(TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/test/cpp")

# List of test cases
set(test_case_sources
    "${TEST_DIR}/test-[-name-].cpp"
)

# Finally invoke jamba_add_vst_plugin
jamba_add_vst_plugin(
    TARGET                "[-target-]" # name of CMake target for the plugin
    RELEASE_FILENAME      "[-filename-]" # filename for the plugin ([-filename-].vst3)
    RELEASE_SNAPSHOTS     "${RES_DIR}/[-snapshot_uuid-]_snapshot.png" "${RES_DIR}/[-snapshot_uuid-]_snapshot_2.0x.png"
    ARCHIVE_ARCHITECTURE  "${JAMBA_ARCHIVE_ARCHITECTURE}"
    TARGETS_PREFIX        "jmb_" # prefix used by all targets directly linked to this plugin
    VST_SOURCES           "${vst_sources}" # the source files that compose the plugin
    INCLUDE_DIRECTORIES   "${VERSION_DIR}" # we add the version folder to the list of includes
    UIDESC                "${RES_DIR}/[-name-].uidesc" # the main xml file for the GUI
    RESOURCES             "${vst_resources}" # the resources for the GUI (png files)
    TEST_CASE_SOURCES     "${test_case_sources}" # the source files containing the test cases
#    TEST_SOURCES          "${CPP_SOURCES}/xxx.cpp" # individual files required for compiling tests but we could add ${vst_sources} if we needed more
    TEST_LINK_LIBRARIES   "jamba" # the libraries needed for linking the tests (if your tests don't depend on Jamba you can comment this out)

)
