cmake_minimum_required(VERSION 3.12)

# build VST 2?
option(JAMBA_ENABLE_VST2 "Use VST2" ON)

# build Audio Unit?
option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" ON)

project(JambaTestPlugin)

# To use local jamba install, uncomment the following line (no download)
set(JAMBA_ROOT_DIR "..")

# To use local googletest install, uncomment the following line (no download) and modify the path accordingly
unset(GOOGLETEST_ROOT_DIR)
#set(GOOGLETEST_ROOT_DIR ../../google/googletest)

# finally we include jamba itself
include(${JAMBA_ROOT_DIR}/jamba.cmake)

set(PLUGIN_MAJOR_VERSION ${JAMBA_MAJOR_VERSION})
set(PLUGIN_MINOR_VERSION ${JAMBA_MINOR_VERSION})
set(PLUGIN_PATCH_VERSION ${JAMBA_PATCH_VERSION})
set(PLUGIN_VERSION "${PLUGIN_MAJOR_VERSION}.${PLUGIN_MINOR_VERSION}.${PLUGIN_PATCH_VERSION}")

set(CPP_SOURCES src/cpp)

configure_file(${CPP_SOURCES}/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

if (JAMBA_ENABLE_VST2)
  set(vst2_sources
      ${CPP_SOURCES}/JambaTestPlugin_VST2.cpp
      )
endif ()

set(vst_sources
    ${CPP_SOURCES}/JambaTestPlugin_VST3.cpp

    ${CPP_SOURCES}/JambaTestPlugin.h
    ${CPP_SOURCES}/JambaTestPluginCIDs.h

    ${CPP_SOURCES}/RT/JambaTestPluginProcessor.h
    ${CPP_SOURCES}/RT/JambaTestPluginProcessor.cpp

    ${CPP_SOURCES}/GUI/JambaTestPluginController.h
    ${CPP_SOURCES}/GUI/JambaTestPluginController.cpp
    )

# VST2 is only defined for macOS and Windows
if (MAC OR WIN)
  set(vst_sources ${vst_sources} ${vst2_sources})
endif ()

set(target pongasoft_JambaTestPlugin)
jamba_add_vst3plugin(${target} "${vst_sources}")
target_include_directories(${target} PUBLIC ${VSTGUI_ROOT}/vstgui4)
target_link_libraries(${target} PRIVATE base sdk vstgui_support jamba)

#------------------------------------------------------------------------
# Resources - add your resources here...
#------------------------------------------------------------------------
jamba_add_vst3_resource(${target} DATA "JambaTestPlugin.uidesc")
# jamba_add_vst3_resource(${target} PNG "background.png")

# this must be called after you are done calling jamba_add_vst3_resource for all resources
jamba_gen_vst3_resources(${target} "JambaTestPlugin")

#------------------------------------------------------------------------
# Create archive (.tgz)
#------------------------------------------------------------------------
jamba_create_archive(${target} "JambaTestPlugin")

#------------------------------------------------------------------------
# Testing - for jamba (NOT the plugin)
#------------------------------------------------------------------------
set(JAMBA_TEST_SOURCES "${JAMBA_ROOT}/test/cpp")
set(JAMBA_TEST_SRC_FILES
    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/Collection/test-CircularBuffer.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/Concurrent/test-concurrent.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/Concurrent/test-concurrent_lockfree.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/test-Lerp.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/test-StringUtils.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/VST/GUI/Views/test-SelfContainedViewListener.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-AudioBuffers.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-ParamConverters.cpp
    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-SampleRateBasedClock.cpp
    )

set(JAMBA_CPP_FOR_TEST_SOURCES
    ${JAMBA_CPP_SOURCES}/pongasoft/logging/logging.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/StringUtils.cpp
    )

set(test_libs base sdk vstgui_support)

jamba_add_test(${target}_test "${JAMBA_TEST_SRC_FILES}" "${JAMBA_CPP_FOR_TEST_SOURCES}" "${test_libs}")

#------------------------------------------------------------------------
# Dev help
#------------------------------------------------------------------------
jamba_dev_scripts(${target})