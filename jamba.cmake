#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------
option(JAMBA_DEBUG_LOGGING "Enable debug logging for jamba framework" OFF)

#-------------------------------------------------------------------------------
# Including VST3 SDK
#-------------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/VST3_SDK.cmake)

#-------------------------------------------------------------------------------
# Defining files to include to generate the library
#-------------------------------------------------------------------------------

set(JAMBA_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(JAMBA_CPP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/cpp)
set(LOGURU_IMPL ${JAMBA_CPP_SOURCES}/pongasoft/logging/logging.cpp)
include_directories(${JAMBA_CPP_SOURCES})

set(JAMBA_SRC_COMMON_DIR ${JAMBA_CPP_SOURCES}/pongasoft/VST/Common)
set(JAMBA_SRC_GUI_DIR ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI)
set(JAMBA_SRC_RT_DIR ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT)

set(JAMBA_sources_h
    ${JAMBA_CPP_SOURCES}/pongasoft/logging/logging.h
    ${JAMBA_CPP_SOURCES}/pongasoft/logging/loguru.hpp

    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Clock/Clock.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Collection/CircularBuffer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Concurrent/Concurrent.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Lerp.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Misc.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/AudioBuffer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/AudioUtils.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Messaging.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/NormalizedState.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/ParamConverters.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/ParamDef.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Parameters.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/ParamSerializers.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/SampleRateBasedClock.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Timer.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTProcessor.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTState.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCx.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxAware.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxMgr.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamSerializers.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIRawVstParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUISerParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIVstParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/VstParameters.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomControlView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomViewCreator.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomViewFactory.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/MomentaryButtonView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/TextEditView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ToggleButtonView.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/DrawContext.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIController.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIState.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIViewState.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Types.h
    )

set(JAMBA_sources_cpp
    ${LOGURU_IMPL}

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Parameters.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/NormalizedState.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTParameter.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTProcessor.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTState.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCx.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxMgr.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxAware.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIRawVstParameter.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/MomentaryButtonView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/TextEditView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ToggleButtonView.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/DrawContext.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIController.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIState.cpp

    )

if (SMTG_CREATE_VST2_VERSION)
  set(JAMBA_vst2_sources
      ${VST3_SDK_ROOT}/public.sdk/source/common/memorystream.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/eventlist.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostclasses.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/parameterchanges.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/processdata.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/vst2wrapper/vst2wrapper.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/vst2wrapper/vst2wrapper.h
      ${VST3_SDK_ROOT}/public.sdk/source/vst2.x/audioeffect.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst2.x/audioeffectx.cpp
      )
endif()

if(JAMBA_DEBUG_LOGGING)
  message(STATUS "Enabling debug logging for jamba framework")
  add_definitions(-DJAMBA_DEBUG_LOGGING)
endif()

###################################################
# jamba_create_archive - Create archive (.tgz)
###################################################
function(jamba_create_archive target plugin_name)
  if(MAC)
    set(ARCHITECTURE "macOS_64bits")
  elseif(WIN)
    set(ARCHITECTURE "win_64bits")
  endif()

  set(ARCHIVE_NAME ${target}-${ARCHITECTURE}-${PLUGIN_VERSION})
  set(ARCHIVE_PATH ${CMAKE_BINARY_DIR}/archive/${ARCHIVE_NAME})

  message(STATUS "Archive path ${ARCHIVE_PATH}.zip")

  add_custom_command(OUTPUT ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/archive/README-${ARCHITECTURE}.txt ${ARCHIVE_PATH}/README.txt
      )

  if(MAC)
    add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/${target}.${VST3_EXTENSION} ${ARCHIVE_PATH}/${plugin_name}.vst3
        DEPENDS ${target}
        DEPENDS ${ARCHIVE_PATH}
        WORKING_DIRECTORY archive
        )
  elseif(WIN)
    add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${ARCHIVE_PATH}/${plugin_name}.vst3
        DEPENDS ${target}
        DEPENDS ${ARCHIVE_PATH}
        WORKING_DIRECTORY archive
        )
  endif()

  if(MAC OR WIN)
    add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
        COMMAND ${CMAKE_COMMAND} -E tar cvf ${ARCHIVE_NAME}.zip --format=zip ${ARCHIVE_PATH}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${ARCHIVE_PATH}
        APPEND
        )

    add_custom_target(archive
        DEPENDS ${ARCHIVE_PATH}.zip
        )
  endif()
endfunction()

###################################################
# jamba_fix_vst2
###################################################
function(jamba_fix_vst2 target)
  if (SMTG_CREATE_VST2_VERSION)
    message(STATUS "${target} will be VST2 compatible")
    if(MAC)
      # fix missing VSTPluginMain symbol when also building VST 2 version
      smtg_set_exported_symbols(${target} "${JAMBA_ROOT}/mac/macexport_vst2.exp")
    endif()
    if (WIN)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
  endif()
endfunction()

###################################################
# jamba_add_test - Testing
###################################################
# Download and unpack googletest at configure time
include(${CMAKE_CURRENT_LIST_DIR}/gtest.cmake)
enable_testing()
include(GoogleTest)

function(jamba_add_test PROJECT_TEST_NAME TEST_CASES_FILES TEST_SOURCES TEST_LIBS)
  message(STATUS "Adding target ${PROJECT_TEST_NAME} for test cases: ${TEST_CASES_FILES}" )

  add_executable(${PROJECT_TEST_NAME} ${TEST_CASES_FILES} ${TEST_SOURCES})
  target_link_libraries(${PROJECT_TEST_NAME} gtest_main ${TEST_LIBS})
  target_include_directories(${PROJECT_TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
  target_include_directories(${PROJECT_TEST_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})

  gtest_add_tests(
      TARGET      ${PROJECT_TEST_NAME}
      TEST_LIST   ${PROJECT_TEST_NAME}_targets
  )
endfunction()

###################################################
# Testing - for this framework
###################################################
message(STATUS "JAMBA_ROOT=${JAMBA_ROOT}" )
file(GLOB_RECURSE TEST_SRC_FILES ${JAMBA_ROOT}/test/cpp/*cpp)
set(test_sources ""
    )

jamba_add_test(jamba_test "${TEST_SRC_FILES}" "${test_sources}" "")
