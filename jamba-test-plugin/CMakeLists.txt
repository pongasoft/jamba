cmake_minimum_required(VERSION 3.14)

# build VST 2?
option(JAMBA_ENABLE_VST2 "Use VST2" ON)

# build Audio Unit?
option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" ON)

project(JambaTestPlugin)

# To use local jamba install, uncomment the following line (no download)
set(JAMBA_ROOT_DIR "..")

# To use local googletest install, uncomment the following line (no download) and modify the path accordingly
unset(GOOGLETEST_ROOT_DIR)
#set(GOOGLETEST_ROOT_DIR ../../../google/googletest)

# finally we include jamba itself
include(${JAMBA_ROOT_DIR}/jamba.cmake)

set(PLUGIN_MAJOR_VERSION ${JAMBA_MAJOR_VERSION})
set(PLUGIN_MINOR_VERSION ${JAMBA_MINOR_VERSION})
set(PLUGIN_PATCH_VERSION ${JAMBA_PATCH_VERSION})
set(PLUGIN_VERSION "${PLUGIN_MAJOR_VERSION}.${PLUGIN_MINOR_VERSION}.${PLUGIN_PATCH_VERSION}")

set(CPP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/cpp)

# We need to recompute the Audio Unit version (AU_PLUGIN_VERSION_HEX) because PLUGIN_MAJOR_VERSION (...) are set
# AFTER including jamba for this plugin only
execute_process(COMMAND bash -c "echo 'obase=16;${PLUGIN_MAJOR_VERSION}*65536+${PLUGIN_MINOR_VERSION}*256+${PLUGIN_PATCH_VERSION}' | bc"
    OUTPUT_VARIABLE AU_PLUGIN_VERSION_HEX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

configure_file(${CPP_SOURCES}/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

if (JAMBA_ENABLE_VST2)
  set(vst2_sources
      ${CPP_SOURCES}/JambaTestPlugin_VST2.cpp
      )
endif ()

set(vst_sources
    ${CPP_SOURCES}/JambaTestPlugin_VST3.cpp

    ${CPP_SOURCES}/JambaTestPluginCIDs.h
    ${CPP_SOURCES}/Model.h
    ${CPP_SOURCES}/Plugin.h
    ${CPP_SOURCES}/Plugin.cpp

    ${CPP_SOURCES}/RT/JambaTestPluginProcessor.h
    ${CPP_SOURCES}/RT/JambaTestPluginProcessor.cpp

    ${CPP_SOURCES}/GUI/JambaTestPluginController.h
    ${CPP_SOURCES}/GUI/JambaTestPluginController.cpp
    )

# VST2 is only defined for macOS and Windows
if (MAC OR WIN)
  set(vst_sources ${vst_sources} ${vst2_sources})
endif ()

# Defining the list of resources
set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/resource)

set(vst_resources
    "${RES_DIR}/button_2frames.png"
    "${RES_DIR}/button_3frames.png"
    "${RES_DIR}/button_4frames.png"
    "${RES_DIR}/image_2frames.png"
    "${RES_DIR}/image_3frames.png"
    "${RES_DIR}/image_4frames.png"
    )

jamba_add_vst3_plugin(
    TARGET pongasoft_JambaTestPlugin
    RELEASE_FILENAME JambaTestPlugin
    TARGETS_PREFIX jmb_
    VST_SOURCES ${vst_sources}
    UIDESC "${RES_DIR}/JambaTestPlugin.uidesc"
    RESOURCES ${vst_resources}
)

#set(target pongasoft_JambaTestPlugin)
#jamba_add_vst3plugin(${target} "${vst_sources}")
#target_include_directories(${target} PUBLIC ${VSTGUI_ROOT}/vstgui4)
#target_link_libraries(${target} PRIVATE base sdk vstgui_support jamba)
#
##------------------------------------------------------------------------
## Resources - add your resources here...
##------------------------------------------------------------------------
#jamba_add_vst3_resource(${target} DATA "JambaTestPlugin.uidesc")
#jamba_add_vst3_resource(${target} PNG "button_2frames.png")
#jamba_add_vst3_resource(${target} PNG "button_3frames.png")
#jamba_add_vst3_resource(${target} PNG "button_4frames.png")
#jamba_add_vst3_resource(${target} PNG "image_2frames.png")
#jamba_add_vst3_resource(${target} PNG "image_3frames.png")
#jamba_add_vst3_resource(${target} PNG "image_4frames.png")
#
## this must be called after you are done calling jamba_add_vst3_resource for all resources
#jamba_gen_vst3_resources(${target} "JambaTestPlugin")
#
##------------------------------------------------------------------------
## Create archive (.tgz)
##------------------------------------------------------------------------
#jamba_create_archive(${target} "JambaTestPlugin")
#
##------------------------------------------------------------------------
## Testing - for jamba (NOT the plugin)
##------------------------------------------------------------------------
#set(JAMBA_TEST_SOURCES "${JAMBA_ROOT}/test/cpp")
#set(JAMBA_TEST_SRC_FILES
#    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/Collection/test-CircularBuffer.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/Concurrent/test-concurrent.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/Concurrent/test-concurrent_lockfree.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/test-Lerp.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/Utils/test-StringUtils.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/GUI/Params/test-GUIParameters.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/GUI/Params/test-ParamAware.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/GUI/Views/test-CustomViewCreator.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/GUI/Views/test-SelfContainedViewListener.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-AudioBuffers.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-AudioUtils.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-ParamConverters.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/test-SampleRateBasedClock.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/Utils/test-Utils.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/Utils/test-FastWriteMemoryStream.cpp
#    ${JAMBA_TEST_SOURCES}/pongasoft/VST/Utils/test-ReadOnlyMemoryStream.cpp
#    )
#
#set(JAMBA_CPP_FOR_TEST_SOURCES
#    ${LOGURU_IMPL}
#    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/StringUtils.cpp
#    )
#
#set(test_libs base sdk jamba vstgui_support)
#
#jamba_add_test(${target}_test "${JAMBA_TEST_SRC_FILES}" "${JAMBA_CPP_FOR_TEST_SOURCES}" "${test_libs}")
#
##------------------------------------------------------------------------
## Dev help
##------------------------------------------------------------------------
#jamba_dev_scripts(${target})
