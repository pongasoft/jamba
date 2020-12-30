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
# This module creates the archive (artifact to install the plugin)
# Must define jamba_create_archive
#------------------------------------------------------------------------
function(jamba_create_archive)
  if (APPLE)
    set_default_value(ARG_ARCHIVE_ARCHITECTURE "macOS_64bits")
  elseif (WIN32)
    set_default_value(ARG_ARCHIVE_ARCHITECTURE "win_64bits")
  endif ()

  set_default_value(CPACK_PACKAGE_NAME                "${ARG_RELEASE_FILENAME}")
  set_default_value(CPACK_PACKAGE_VERSION_MAJOR       "${PLUGIN_MAJOR_VERSION}")
  set_default_value(CPACK_PACKAGE_VERSION_MINOR       "${PLUGIN_MINOR_VERSION}")
  set_default_value(CPACK_PACKAGE_VERSION_PATCH       "${PLUGIN_PATCH_VERSION}")
  set_default_value(CPACK_SYSTEM_NAME                 "${ARG_ARCHIVE_ARCHITECTURE}")
  set_default_value(ARG_ARCHIVE_FILENAME              "${CPACK_PACKAGE_NAME}-${CPACK_SYSTEM_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

  # Generating a file which CPack will read to handle naming the archive differently based on
  # build config
  file(WRITE "${CMAKE_BINARY_DIR}/CPackProjectConfig.cmake" "
set(CPACK_PACKAGE_FILE_NAME \"${ARG_ARCHIVE_FILENAME}\")

if(\"\${CPACK_BUILD_CONFIG}\" STREQUAL \"Debug\")
  set(CPACK_PACKAGE_FILE_NAME \"\${CPACK_PACKAGE_FILE_NAME}_Debug\")
endif()
")

  set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackProjectConfig.cmake")

  set(CPACK_GENERATOR ZIP)
  set(CPACK_VERBATIM_VARIABLES TRUE)
  set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
  set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

  # Add the doc folder at the root if it exists
  if(EXISTS "${ARG_ARCHIVE_ROOT_DIR}")
    list(APPEND CPACK_INSTALLED_DIRECTORIES "${ARG_ARCHIVE_ROOT_DIR}" ".")
  endif()

  include(CPack)

  cpack_add_component(vst3)

  if (JAMBA_ENABLE_VST2)
    cpack_add_component(vst2)
  endif()

  if (APPLE AND JAMBA_ENABLE_AUDIO_UNIT AND XCODE)
    cpack_add_component(au)
  endif()

  add_custom_target("${ARG_TARGETS_PREFIX}create_archive"
      COMMAND ${CMAKE_COMMAND} -E echo "Creating archive..."
      COMMAND "${CMAKE_CPACK_COMMAND}" --verbose -G ZIP -C $<CONFIG>
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" # CPackConfig.cmake is created there
      DEPENDS "${ARG_TARGETS_PREFIX}build_all"
      )

endfunction()
