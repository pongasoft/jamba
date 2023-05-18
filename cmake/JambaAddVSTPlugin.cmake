# Copyright (c) 2020 pongasoft
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

#------------------------------------------------------------------------
# This module defines the Jamba Framework which contains the main
# jamba_add_vst_plugin function to be called by the plugin CMakeFile.txt
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# internal_jamba_create_install_target
#------------------------------------------------------------------------
function(internal_jamba_create_install_target component src dstDir extension)
  if(APPLE)
    # Cannot use generator expression in install => must define multiple with different CONFIGURATIONS
    install(DIRECTORY "${src}/"
            COMPONENT "${component}"
            CONFIGURATIONS Debug
            DESTINATION "${dstDir}/${ARG_RELEASE_FILENAME}_Debug.${extension}"
            )
    install(DIRECTORY "${src}/"
            COMPONENT "${component}"
            CONFIGURATIONS Release
            DESTINATION "${dstDir}/${ARG_RELEASE_FILENAME}.${extension}"
        )
  elseif(WIN32)
    if(SMTG_CREATE_BUNDLE_FOR_WINDOWS)
      install(DIRECTORY "${src}/"
              COMPONENT "${component}"
              CONFIGURATIONS Debug
              DESTINATION "${dstDir}/${ARG_RELEASE_FILENAME}_Debug.${extension}"
              )
      install(DIRECTORY "${src}/"
              COMPONENT "${component}"
              CONFIGURATIONS Release
              DESTINATION "${dstDir}/${ARG_RELEASE_FILENAME}.${extension}"
              )
    else()
      install(FILES "${src}"
              COMPONENT "${component}"
              DESTINATION "${dstDir}"
              CONFIGURATIONS Debug
              RENAME "${ARG_RELEASE_FILENAME}_Debug.${extension}"
              )
      install(FILES "${src}"
              COMPONENT "${component}"
              DESTINATION "${dstDir}"
              CONFIGURATIONS Release
              RENAME "${ARG_RELEASE_FILENAME}.${extension}"
              )
    endif()
  endif()

  set(PLUGIN_FILENAME "${ARG_RELEASE_FILENAME}$<$<CONFIG:Debug>:_Debug>.${extension}")

  #------------------------------------------------------------------------
  # target install_<component>
  # Invokes cmake --install with right component and config
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}install_${component}"
      COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --component "${component}" --config $<CONFIG>
      DEPENDS "${ARG_TARGETS_PREFIX}build_${component}"
      COMMAND ${CMAKE_COMMAND} -E echo "Installed ${component} plugin under ${CMAKE_INSTALL_PREFIX}/${dstDir}/${PLUGIN_FILENAME}"
      )

  #------------------------------------------------------------------------
  # target uninstall_<component>
  # Deletes the folder / file installed with install_<component>
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}uninstall_${component}"
      COMMAND ${CMAKE_COMMAND} -E rm -r -f "${CMAKE_INSTALL_PREFIX}/${dstDir}/${PLUGIN_FILENAME}"
      COMMAND ${CMAKE_COMMAND} -E echo "Removed ${component} plugin [${PLUGIN_FILENAME}] from ${CMAKE_INSTALL_PREFIX}/${dstDir}"
      )
endfunction()

#------------------------------------------------------------------------
# set_default_value - convenient macro
#------------------------------------------------------------------------
macro(set_default_value name default_value)
  if(NOT DEFINED ${name})
    set(${name} ${default_value})
  endif()
endmacro()

#------------------------------------------------------------------------
# jamba_add_vst_plugin
#------------------------------------------------------------------------
function(jamba_add_vst_plugin)
  #------------------------------------------------------------------------
  # Argument parsing / default values
  #------------------------------------------------------------------------
  set(options "")
  set(oneValueArgs TARGET TEST_TARGET UIDESC RELEASE_FILENAME ARCHIVE_FILENAME ARCHIVE_ARCHITECTURE TARGETS_PREFIX MAC_INFO_PLIST_FILE PYTHON3_EXECUTABLE INSTALL_PREFIX_DIR ARCHIVE_ROOT_DIR)
  set(multiValueArgs VST_SOURCES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS COMPILE_OPTIONS LINK_LIBRARIES LINK_OPTIONS
                     RESOURCES RELEASE_SNAPSHOTS
                     TEST_CASE_SOURCES TEST_SOURCES TEST_INCLUDE_DIRECTORIES TEST_COMPILE_DEFINITIONS TEST_COMPILE_OPTIONS TEST_LINK_LIBRARIES)
  cmake_parse_arguments(
      "ARG" # prefix
      "${options}" # options
      "${oneValueArgs}" # single values
      "${multiValueArgs}" # multiple values
      ${ARGN}
  )

  #------------------------------------------------------------------------
  # Sanity check
  #------------------------------------------------------------------------
  if(NOT PROJECT_VERSION)
    if(NOT JAMBA_PROJECT_VERSION_WARNING_DISABLED)
      message(WARNING "project VERSION not set. Consider setting it with PLUGIN_VERSION (ex: project(xxx VERSION \${PLUGIN_VERSION})")
    endif()
    set(PROJECT_VERSION "${PLUGIN_VERSION}")
  endif()

  # Make sure ARG_TARGET has a value (default to project name if not provided)
  set_default_value(ARG_TARGET "${CMAKE_PROJECT_NAME}")
  set_default_value(ARG_UIDESC "${CMAKE_CURRENT_LIST_DIR}/resource/${ARG_TARGET}.uidesc")
  set_default_value(ARG_TEST_TARGET "${ARG_TARGET}_test")
  set_default_value(ARG_RELEASE_FILENAME "${ARG_TARGET}")
  set_default_value(ARG_MAC_INFO_PLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/mac/Info.plist")
  set_default_value(ARG_ARCHIVE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/archive")
  if(APPLE)
    set_default_value(ARG_INSTALL_PREFIX_DIR "$ENV{HOME}/Library/Audio/Plug-Ins")
  elseif(WIN32)
    set_default_value(ARG_INSTALL_PREFIX_DIR "C:/Program\ Files")
  endif()

  # where the plugins are installed
  set(CMAKE_INSTALL_PREFIX "${ARG_INSTALL_PREFIX_DIR}" CACHE PATH "Forcing CMAKE_INSTALL_PREFIX" FORCE)

  # Adds the VST3 plugin
  include(JambaAddVST3Plugin)
  jamba_add_vst3_plugin()

  # Add all resources (uidesc + graphics)
  include(JambaAddAllResources)
  jamba_add_all_resources()

  # Adds Audio Unit (if enabled)
  if (APPLE AND JAMBA_ENABLE_AUDIO_UNIT)
    include(JambaAddAudioUnitPlugin)
    jamba_add_au_plugin()
  endif()

  # Adds jamba.sh / jamba.bat / jamba.py
  if(JAMBA_ENABLE_DEV_SCRIPT)
    include(JambaDevScript)
    jamba_add_dev_script()
  endif()

  # Optionally setup testing
  if(JAMBA_ENABLE_TESTING)
    include(JambaAddTest)
    jamba_add_test()
  endif()

  # Optionally create archive
  if(JAMBA_ENABLE_CREATE_ARCHIVE)
    include(JambaCreateArchive)
    jamba_create_archive()
  endif()

endfunction()
