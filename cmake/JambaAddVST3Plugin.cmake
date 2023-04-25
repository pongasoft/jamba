# Copyright (c) 2020-2023 pongasoft
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
# This module adds the VST3 plugin
# Must define jamba_add_vst3_plugin
#------------------------------------------------------------------------
function(jamba_add_vst3_plugin)
  internal_jamba_add_vst3_plugin()
  internal_jamba_add_vst3_targets()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_vst3_plugin
#------------------------------------------------------------------------
function(internal_jamba_add_vst3_plugin)
  smtg_add_vst3plugin("${ARG_TARGET}" "${vst3sdk_SOURCE_DIR}" "${ARG_VST_SOURCES}")

  # We need to link with jamba (which defines what it needs to link with)
  target_link_libraries("${ARG_TARGET}" PUBLIC "jamba" "${ARG_LINK_LIBRARIES}")

  # Making sure that all libraries included are setup for universal build
  foreach(lib IN LISTS ARG_LINK_LIBRARIES)
    smtg_target_setup_universal_binary("${lib})")
  endforeach()

  # Extra includes?
  if(ARG_INCLUDE_DIRECTORIES)
    target_include_directories("${ARG_TARGET}" PUBLIC "${ARG_INCLUDE_DIRECTORIES}")
  endif()

  # Extra compile definitions?
  if(ARG_COMPILE_DEFINITIONS)
    target_compile_definitions("${ARG_TARGET}" PUBLIC "${ARG_COMPILE_DEFINITIONS}")
  endif()

  # Extra compile options?
  if(ARG_COMPILE_OPTIONS)
    target_compile_options("${ARG_TARGET}" PUBLIC "${ARG_COMPILE_OPTIONS}")
  endif()

  if (XCODE)
    # $<TARGET_BUNDLE_DIR:tgt> relies on this property so we must set it (not set by VST3 SDK when XCODE)
    set_target_properties(${ARG_TARGET} PROPERTIES BUNDLE_EXTENSION "vst3")
  endif ()

  if (APPLE)
    # In the case of Apple we add bundleEntry and bundleExit
    set(LINK_OPTIONS
        "LINKER:-exported_symbol,_GetPluginFactory"
        "LINKER:-exported_symbol,_bundleEntry"
        "LINKER:-exported_symbol,_bundleExit"
        )
  elseif (WIN32)
    # In the case of Windows we add InitDLL and ExitDLL
    set(LINK_OPTIONS
        "/export:GetPluginFactory"
        "/export:InitDll"
        "/export:ExitDll"
        )
  endif ()

  target_link_options(${ARG_TARGET} PUBLIC ${LINK_OPTIONS} ${ARG_LINK_OPTIONS})

endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_vst3_targets
#------------------------------------------------------------------------
function(internal_jamba_add_vst3_targets)
  #------------------------------------------------------------------------
  # build_vst3 target | can be changed by setting BUILD_VST3_TARGET before calling this function
  #------------------------------------------------------------------------
  if (NOT BUILD_VST3_TARGET)
    set(BUILD_VST3_TARGET "${ARG_TARGET}")
  endif ()
  add_custom_target("${ARG_TARGETS_PREFIX}build_vst3" DEPENDS "${BUILD_VST3_TARGET}")
  add_custom_target("${ARG_TARGETS_PREFIX}build_all" DEPENDS "${ARG_TARGETS_PREFIX}build_vst3")

  # add install targets so that they can be invoked by the scripts (and from the IDE)
  if (APPLE)
    set(VST3_PLUGIN_SRC $<TARGET_BUNDLE_DIR:${ARG_TARGET}>)
    set(VST3_PLUGIN_DST_DIR "VST3")
    set(VST3_PLUGIN_EXTENSION "vst3")
  elseif (WIN32)
    if(SMTG_CREATE_BUNDLE_FOR_WINDOWS)
      get_target_property(PLUGIN_PACKAGE_PATH ${ARG_TARGET} SMTG_PLUGIN_PACKAGE_PATH)
      set(VST3_PLUGIN_SRC "${PLUGIN_PACKAGE_PATH}")
    else()
      set(VST3_PLUGIN_SRC $<TARGET_FILE:${ARG_TARGET}>)
    endif()
    set(VST3_PLUGIN_DST_DIR "Common\ Files/VST3")
    set(VST3_PLUGIN_EXTENSION "vst3")
  endif ()

  #------------------------------------------------------------------------
  # install_vst3 target
  # uninstall_vst3 target
  # install_vst target (for installing both vst2 and vst3)
  # install_all target (for installing vst2/vst3 and audio unit)
  # uninstall_all target (for uninstalling all)
  #------------------------------------------------------------------------
  internal_jamba_create_install_target("vst3" "${VST3_PLUGIN_SRC}" "${VST3_PLUGIN_DST_DIR}" "${VST3_PLUGIN_EXTENSION}")
  add_custom_target("${ARG_TARGETS_PREFIX}install_vst" DEPENDS "${ARG_TARGETS_PREFIX}install_vst3")
  add_custom_target("${ARG_TARGETS_PREFIX}install_all" DEPENDS "${ARG_TARGETS_PREFIX}install_vst3")
  add_custom_target("${ARG_TARGETS_PREFIX}uninstall_all" DEPENDS "${ARG_TARGETS_PREFIX}uninstall_vst3")

  #------------------------------------------------------------------------
  # run_validator target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}run_validator"
      COMMAND $<TARGET_FILE:validator> ${VST3_PLUGIN_SRC}
      DEPENDS ${ARG_TARGET} validator)

  #------------------------------------------------------------------------
  # run_editor target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}run_editor"
      COMMAND $<TARGET_FILE:editorhost> "${VST3_PLUGIN_SRC}"
      DEPENDS ${ARG_TARGET} editorhost)

  #------------------------------------------------------------------------
  # run_info target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}run_info"
      COMMAND $<TARGET_FILE:moduleinfotool> -create -version "${PROJECT_VERSION}" -path "${VST3_PLUGIN_SRC}"
      DEPENDS ${ARG_TARGET} moduleinfotool)

  #------------------------------------------------------------------------
  # run_inspector target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}run_inspector"
      COMMAND $<TARGET_FILE:VST3Inspector>
      DEPENDS VST3Inspector)

  #------------------------------------------------------------------------
  # clean_all target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}clean_all"
      COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target clean
      )
endfunction()
