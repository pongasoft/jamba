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
# This module adds all the resources (images and .uidesc and .rc to the build)
# Must define jamba_add_all_resources
#------------------------------------------------------------------------
function(jamba_add_all_resources)
  set(JAMBA_VST3_RESOURCES_RC "")
  set(JAMBA_VST3_RESOURCES_FILE_DEPENDENCY "")

  get_filename_component(UIDESC_FILENAME ${ARG_UIDESC} NAME_WLE)
  get_filename_component(UIDESC_DIR ${ARG_UIDESC} DIRECTORY)

  internal_jamba_add_resource("${ARG_UIDESC}" "DATA" "" JAMBA_VST3_RESOURCES_RC "" JAMBA_VST3_RESOURCES_FILE_DEPENDENCY)
  internal_jamba_add_resources("${JAMBA_VST3_RESOURCES_RC}" JAMBA_VST3_RESOURCES_RC "${JAMBA_VST3_RESOURCES_FILE_DEPENDENCY}" JAMBA_VST3_RESOURCES_FILE_DEPENDENCY)

  if (APPLE)
    # processes replacement properties in ARG_MAC_INFO_PLIST_FILE
    set(PLUGIN_INFO_PLIST_FILE "${CMAKE_BINARY_DIR}/PluginInfo.plist")
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "${ARG_TARGET}")
    configure_file("${ARG_MAC_INFO_PLIST_FILE}" "${PLUGIN_INFO_PLIST_FILE}" @ONLY)
    smtg_target_set_bundle(${ARG_TARGET} INFOPLIST "${PLUGIN_INFO_PLIST_FILE}" PREPROCESS)
    if(XCODE)
      # make sure that "version.h" is in the path
      set_target_properties(${ARG_TARGET} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_OTHER_PREPROCESSOR_FLAGS "-I ${CMAKE_BINARY_DIR}/generated")
    else()
      # when not using XCode, we need to use PLUGIN_INFO_PLIST_FILE as well
      set_target_properties(${ARG_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${PLUGIN_INFO_PLIST_FILE}")
    endif()
  elseif (WIN32)
    list(JOIN JAMBA_VST3_RESOURCES_RC "\n" JAMBA_VST3_RESOURCES_RC)
    file(WRITE "${CMAKE_BINARY_DIR}/generated/vst3_resources.rc" "${JAMBA_VST3_RESOURCES_RC}\n")
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/generated/plugin_resources.rc"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${UIDESC_DIR}/${UIDESC_FILENAME}.rc" "${CMAKE_BINARY_DIR}/generated/plugin_resources.rc"
        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/generated/plugin_resources.rc"
        DEPENDS "${JAMBA_VST3_RESOURCES_FILE_DEPENDENCY}"
        VERBATIM
        )
    target_sources(${ARG_TARGET} PRIVATE "${CMAKE_BINARY_DIR}/generated/plugin_resources.rc")
  endif ()

  if(ARG_RELEASE_SNAPSHOTS)
    foreach(snapshot IN LISTS ARG_RELEASE_SNAPSHOTS)
      smtg_add_vst3_snapshot(${ARG_TARGET} "${snapshot}")
    endforeach()
  endif()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_resources
#------------------------------------------------------------------------
function(internal_jamba_add_resources inRCContentList outRCContentList inFileDependencyList outFileDependencyList)
  foreach(resource IN LISTS ARG_RESOURCES)
    get_filename_component(type "${resource}" LAST_EXT)
    string(SUBSTRING "${type}" 1 -1 type)
    string(TOUPPER "${type}" type)
    internal_jamba_add_resource("${resource}" "${type}" "${inRCContentList}" inRCContentList "${inFileDependencyList}" inFileDependencyList)
  endforeach()
  set("${outRCContentList}" "${inRCContentList}" PARENT_SCOPE)
  set("${outFileDependencyList}" "${inFileDependencyList}" PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_resources
#------------------------------------------------------------------------
function(internal_jamba_add_resource resource type inRCContentList outRCContentList inFileDependencyList outFileDependencyList)
  get_filename_component(filename "${resource}" NAME)
  smtg_target_add_plugin_resources(${ARG_TARGET} RESOURCES "${resource}")
  file(TO_NATIVE_PATH "${resource}" JAMBA_VST3_RESOURCE_PATH)
  list(APPEND inFileDependencyList "${JAMBA_VST3_RESOURCE_PATH}")
  string(REPLACE "\\" "\\\\" JAMBA_VST3_RESOURCE_PATH "${JAMBA_VST3_RESOURCE_PATH}")
  list(APPEND inRCContentList "${filename}\t${type}\t\"${JAMBA_VST3_RESOURCE_PATH}\"")
  set("${outRCContentList}" "${inRCContentList}" PARENT_SCOPE)
  set("${outFileDependencyList}" "${inFileDependencyList}" PARENT_SCOPE)
endfunction()
