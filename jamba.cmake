cmake_minimum_required (VERSION 3.12)

# JAMBA_ROOT
set(JAMBA_ROOT ${CMAKE_CURRENT_LIST_DIR})

#------------------------------------------------------------------------
# Version
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

get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

#------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------
option(JAMBA_DEBUG_LOGGING "Enable debug logging for jamba framework" OFF)

#-------------------------------------------------------------------------------
# Platform Detection
#-------------------------------------------------------------------------------

if(APPLE)
  set(MAC TRUE)
elseif(WIN32)
  set(WIN TRUE)
endif()

#------------------------------------------------------------------------
# Compiler options
#------------------------------------------------------------------------
if(WIN)
  message(STATUS "Adding compiler options")
  add_compile_options("/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING" "/EHsc" "/D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING" "/D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING")
endif ()

#------------------------------------------------------------------------
# Including VST3 SDK
#------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)

# options set before including the SDK
option(SMTG_CREATE_VST2_VERSION "" ${JAMBA_ENABLE_VST2})
option(SMTG_ENABLE_TARGET_VARS_LOG "" OFF) # disable dump variables
option(SMTG_RUN_VST_VALIDATOR "" OFF) # disable validator (explicit validate step)
option(SMTG_CREATE_PLUGIN_LINK "" OFF) # disable link (explicit install step)
option(SMTG_ADD_VST3_PLUGINS_SAMPLES "" OFF) # disable plugin samples

#------------------------------------------------------------------------
# Audio Unit
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

# Adding cmake/modules to cmake path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

# Including VST3 SDK
include(VST3_SDK)

# optionally including VST2 SDK
if(JAMBA_ENABLE_VST2)
  include(VST2_SDK)
endif ()

# Defining VST3_OUTPUT_DIR which is the location where the VST3 artifact is hosted
set(VST3_OUTPUT_DIR ${CMAKE_BINARY_DIR}/VST3)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src jamba)

#------------------------------------------------------------------------
# jamba_create_archive - Create archive (.tgz)
#------------------------------------------------------------------------
function(jamba_create_archive target plugin_name)
  if (MAC)
    set(ARCHITECTURE "macOS_64bits")
  elseif (WIN)
    set(ARCHITECTURE "win_64bits")
  endif ()

  set(JAMBA_RELEASE_FILENAME "${plugin_name}" PARENT_SCOPE)

  set(ARCHIVE_NAME ${target}-${ARCHITECTURE}-${PLUGIN_VERSION})
  set(ARCHIVE_PATH ${CMAKE_BINARY_DIR}/archive/${ARCHIVE_NAME})

  add_custom_command(OUTPUT ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/archive/README-${ARCHITECTURE}.txt ${ARCHIVE_PATH}/README.txt
      )

  if (MAC)
    if (XCODE)
      add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}.${VST3_EXTENSION} ${ARCHIVE_PATH}/${plugin_name}.vst3
          DEPENDS ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}.${VST3_EXTENSION}
          DEPENDS ${ARCHIVE_PATH}
          WORKING_DIRECTORY archive
          )
      if (JAMBA_ENABLE_AUDIO_UNIT)
        add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}_au.component ${ARCHIVE_PATH}/${plugin_name}.component
            DEPENDS ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}_au.component
            DEPENDS ${ARCHIVE_PATH}
            WORKING_DIRECTORY archive
            APPEND
            )
      endif ()
    else ()
      add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/${target}.${VST3_EXTENSION} ${ARCHIVE_PATH}/${plugin_name}.vst3
          DEPENDS ${VST3_OUTPUT_DIR}/${target}.${VST3_EXTENSION}
          DEPENDS ${ARCHIVE_PATH}
          WORKING_DIRECTORY archive
          )
    endif ()
  elseif (WIN)
    add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${ARCHIVE_PATH}/${plugin_name}.vst3
        DEPENDS ${target}
        DEPENDS ${ARCHIVE_PATH}
        WORKING_DIRECTORY archive
        )
  endif ()

  add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
      COMMAND ${CMAKE_COMMAND} -E tar cvf ${ARCHIVE_NAME}$<$<CONFIG:Debug>:_Debug>.zip --format=zip ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E echo "Archive available under ${CMAKE_BINARY_DIR}/archive/${ARCHIVE_NAME}$<$<CONFIG:Debug>:_Debug>.zip"
      APPEND
      )

  add_custom_target(archive
      DEPENDS ${ARCHIVE_PATH}.zip
      )
endfunction()

#------------------------------------------------------------------------
# jamba_add_vst3_resource
#------------------------------------------------------------------------
set(JAMBA_VST3_RESOURCES_RC "")
if (NOT JAMBA_RESOURCE_DIR)
  set(JAMBA_RESOURCE_DIR "${PROJECT_SOURCE_DIR}/resource")
endif ()
function(jamba_add_vst3_resource target type filename)
  smtg_add_vst3_resource(${target} "${JAMBA_RESOURCE_DIR}/${filename}" "")
  file(TO_NATIVE_PATH "${JAMBA_RESOURCE_DIR}/${filename}" JAMBA_VST3_RESOURCE_PATH)
  string(REPLACE "\\" "\\\\" JAMBA_VST3_RESOURCE_PATH "${JAMBA_VST3_RESOURCE_PATH}")
  set(JAMBA_VST3_RESOURCES_RC "${JAMBA_VST3_RESOURCES_RC}\n${filename}\t${type}\t\"${JAMBA_VST3_RESOURCE_PATH}\"" PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------
# jamba_gen_vst3_resources
#------------------------------------------------------------------------
function(jamba_gen_vst3_resources target name)
  if (MAC)
    if (NOT JAMBA_VST3_PLUGIN_MAC_INFO_PLIST)
      set(JAMBA_VST3_PLUGIN_MAC_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/mac/Info.plist")
    endif ()
    smtg_set_bundle(${target} INFOPLIST "${JAMBA_VST3_PLUGIN_MAC_INFO_PLIST}" PREPROCESS)
  elseif (WIN)
    jamba_gen_vst3_resource_rc("${CMAKE_BINARY_DIR}/generated/vst3_resources.rc")
    target_sources(${target} PRIVATE resource/${name}.rc)
  endif ()
endfunction()


#------------------------------------------------------------------------
# jamba_gen_vst3_resource_rc
#------------------------------------------------------------------------
function(jamba_gen_vst3_resource_rc filepath)
  file(WRITE ${filepath} ${JAMBA_VST3_RESOURCES_RC})
endfunction()

#------------------------------------------------------------------------
# jamba_add_vst3plugin
#------------------------------------------------------------------------
function(jamba_add_vst3plugin target vst_sources)

  # 1. Adds the VST3 plugin
  smtg_add_vst3plugin(${target} ${VST3_SDK_ROOT} ${vst_sources})

  # 2. Implement a fix for VST2
  jamba_fix_vst2(${target})

  # 3. If Audio Unit enabled => add audio unit
  if (MAC AND JAMBA_ENABLE_AUDIO_UNIT)

    set(JAMBA_VST3_PLUGIN_TARGET "${target}")
    # add the wrapper
    add_subdirectory(${JAMBA_ROOT}/audio-unit/auwrapper auwrapper)
    # add the plugin
    if (NOT AU_PLUGIN_ROOT)
      set(AU_PLUGIN_ROOT ${PROJECT_SOURCE_DIR}/audio-unit)
    endif ()
    add_subdirectory(${JAMBA_ROOT}/audio-unit/plugin auplugin)
  endif ()

endfunction()

#------------------------------------------------------------------------
# jamba_fix_vst2
#------------------------------------------------------------------------
function(jamba_fix_vst2 target)
  if (JAMBA_ENABLE_VST2)
    message(STATUS "${target} will be VST2 compatible")
    if (MAC)
      # fix missing VSTPluginMain symbol when also building VST 2 version
      smtg_set_exported_symbols(${target} "${JAMBA_ROOT}/mac/macexport_vst2.exp")
    endif ()
    if (WIN)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif ()
  endif ()
endfunction()

#------------------------------------------------------------------------
# jamba_dev_scripts
#------------------------------------------------------------------------
function(jamba_dev_scripts target)
  if (NOT JAMBA_RELEASE_FILENAME)
    message(WARNING "JAMBA_RELEASE_FILENAME not set... you should call jamba_create_archive first!")
    set(JAMBA_RELEASE_FILENAME "${target}")
  endif ()
  if (MAC)
    if (GENERATOR_IS_MULTI_CONFIG)
      configure_file(${JAMBA_ROOT}/scripts/jamba_multi.sh.in ${CMAKE_BINARY_DIR}/jamba.sh @ONLY)
    else ()
      configure_file(${JAMBA_ROOT}/scripts/jamba_single.sh.in ${CMAKE_BINARY_DIR}/jamba.sh @ONLY)
    endif ()
  endif ()
  if (WIN)
    configure_file(${JAMBA_ROOT}/scripts/jamba.bat.in ${CMAKE_BINARY_DIR}/jamba.bat @ONLY)
  endif ()

  # build_vst3 target / can be changed by setting BUILD_VST3_TARGET before calling this function
  if (NOT BUILD_VST3_TARGET)
    set(BUILD_VST3_TARGET "${target}")
  endif ()
  add_custom_target(build_vst3 DEPENDS "${BUILD_VST3_TARGET}")

  # test_vst3 target / can be changed by setting TEST_VST3_TARGET before calling this function
  if (NOT TEST_VST3_TARGET)
    set(TEST_VST3_TARGET "${target}_test")
  endif ()
  add_custom_target(test_vst3 DEPENDS "${TEST_VST3_TARGET}")

  # add install targets so that they can be invoked by the scripts (and from the IDE)
  set(PLUGIN_FILENAME ${JAMBA_RELEASE_FILENAME}$<$<CONFIG:Debug>:_Debug>)

  if (MAC)
    ### VST3
    set(VST3_PLUGIN_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3")
    if (GENERATOR_IS_MULTI_CONFIG)
      set(VST3_PLUGIN_SRC "${VST3_OUTPUT_DIR}/$<CONFIG>/${target}.vst3")
    else ()
      set(VST3_PLUGIN_SRC "${VST3_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/${target}.vst3")
    endif ()
    set(VST3_PLUGIN_DST "${VST3_PLUGIN_DIR}/${PLUGIN_FILENAME}.vst3")

    ### install_vst3 target
    add_custom_target(install_vst3
        COMMAND ${CMAKE_COMMAND} -E make_directory ${VST3_PLUGIN_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${VST3_PLUGIN_DST}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_PLUGIN_SRC} ${VST3_PLUGIN_DST}
        DEPENDS build_vst3
        )

    ### VST2 ??
    if (JAMBA_ENABLE_VST2)
      set(VST2_PLUGIN_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST")
      set(VST2_PLUGIN_SRC "${VST3_PLUGIN_SRC}")
      set(VST2_PLUGIN_DST "${VST2_PLUGIN_DIR}/${PLUGIN_FILENAME}.vst")

      ### install_vst2 target
      add_custom_target(install_vst2
          COMMAND ${CMAKE_COMMAND} -E make_directory ${VST2_PLUGIN_DIR}
          COMMAND ${CMAKE_COMMAND} -E remove_directory ${VST2_PLUGIN_DST}
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST2_PLUGIN_SRC} ${VST2_PLUGIN_DST}
          DEPENDS build_vst3
          )
    endif ()

    ### AUDIO UNIT ??
    if (JAMBA_ENABLE_AUDIO_UNIT)
      if (XCODE) # audio unit requires XCode... so not available with normal Makefile build (ex: CLion)
        if (NOT BUILD_AU_TARGET)
          set(BUILD_AU_TARGET "${target}_au")
        endif ()
        add_custom_target(build_au DEPENDS "${BUILD_AU_TARGET}")

        set(AU_PLUGIN_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/Components")
        set(AU_PLUGIN_DST "${AU_PLUGIN_DIR}/${PLUGIN_FILENAME}.component")

        ### install_au target
        add_custom_target(install_au
            COMMAND ${CMAKE_COMMAND} -E make_directory ${AU_PLUGIN_DIR}
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${AU_PLUGIN_DST}
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${VST3_OUTPUT_DIR}/$<CONFIG>/${target}_au.component" ${AU_PLUGIN_DST}
            DEPENDS build_au
            )
      endif()
    endif()
  elseif (WIN)
    ### VST3
    set(VST3_PLUGIN_DIR "C:/Program\ Files/Common\ Files/VST3")
    set(VST3_PLUGIN_DST "${VST3_PLUGIN_DIR}/${PLUGIN_FILENAME}.vst3")

    ### install_vst3 target
    add_custom_target(install_vst3
        COMMAND ${CMAKE_COMMAND} -E make_directory ${VST3_PLUGIN_DIR}
        COMMAND ${CMAKE_COMMAND} -E remove -f ${VST3_PLUGIN_DST}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${VST3_PLUGIN_DST}
        COMMENT "#### You might need to run as administrator if the command fails"
        DEPENDS build_vst3
        )

    add_custom_command(TARGET install_vst3 
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E echo "#### VST3 plugin installed under ${VST3_PLUGIN_DST}"
                      )

    ### VST2 ??
    if (JAMBA_ENABLE_VST2)
      set(VST2_PLUGIN_DIR "C:/Program\ Files/VSTPlugins")
      set(VST2_PLUGIN_DST "${VST2_PLUGIN_DIR}/${PLUGIN_FILENAME}.dll")

      ### install_vst2 target
      add_custom_target(install_vst2
          COMMAND ${CMAKE_COMMAND} -E make_directory ${VST2_PLUGIN_DIR}
          COMMAND ${CMAKE_COMMAND} -E remove -f ${VST2_PLUGIN_DST}
          COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${VST2_PLUGIN_DST}
          COMMENT "#### You might need to run as administrator if the command fails"
          DEPENDS build_vst3
          )

      add_custom_command(TARGET install_vst2 
                         POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E echo "#### VST2 plugin installed under ${VST2_PLUGIN_DST}"
                        )
    endif ()
  endif ()
endfunction()

#------------------------------------------------------------------------
# Testing
#------------------------------------------------------------------------
# Download and unpack googletest at configure time
include(gtest)
enable_testing()
include(GoogleTest)

#------------------------------------------------------------------------
# jamba_add_test - Testing
#------------------------------------------------------------------------
function(jamba_add_test PROJECT_TEST_NAME TEST_CASES_FILES TEST_SOURCES TEST_LIBS)
  message(STATUS "Adding target ${PROJECT_TEST_NAME} for test cases: ${TEST_CASES_FILES}")

  if (WIN)
    set(WIN_SOURCES "${JAMBA_ROOT}/windows/testmain.cpp")
  endif ()

  add_executable(${PROJECT_TEST_NAME} ${TEST_CASES_FILES} ${TEST_SOURCES} ${WIN_SOURCES})
  target_link_libraries(${PROJECT_TEST_NAME} gtest_main ${TEST_LIBS})
  target_include_directories(${PROJECT_TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
  target_include_directories(${PROJECT_TEST_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})

  gtest_add_tests(
      TARGET ${PROJECT_TEST_NAME}
      TEST_LIST ${PROJECT_TEST_NAME}_targets
  )
endfunction()
