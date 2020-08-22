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
  smtg_add_vst3plugin("${ARG_TARGET}" "${VST3_SDK_ROOT}" "${ARG_VST_SOURCES}")

  # We need to link with jamba (which defines what it needs to link with)
  target_link_libraries("${ARG_TARGET}" PUBLIC "jamba" "${ARG_LINK_LIBRARIES}")

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
    set(VST3_PLUGIN_SRC $<TARGET_FILE:${ARG_TARGET}>)
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
      COMMAND $<TARGET_FILE:editorhost> ${VST3_PLUGIN_SRC}
      DEPENDS ${ARG_TARGET} editorhost)

  #------------------------------------------------------------------------
  # clean_all target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}clean_all"
      COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target clean
      )
endfunction()
