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
# This module will build the audio unit plugin wrapper
# Must define jamba_add_au_plugin
#------------------------------------------------------------------------
function(jamba_add_au_plugin)
  internal_jamba_add_au_plugin()
  internal_jamba_add_au_targets()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_au_plugin
#------------------------------------------------------------------------
function(internal_jamba_add_au_plugin)
  if (XCODE)
    set(JAMBA_VST3_PLUGIN_TARGET "${ARG_TARGET}")
    # add the wrapper
    add_subdirectory(${JAMBA_ROOT}/audio-unit/auwrapper auwrapper)
    # add the plugin
    if (NOT AU_PLUGIN_ROOT)
      set(AU_PLUGIN_ROOT ${PROJECT_SOURCE_DIR}/audio-unit)
    endif ()
    add_subdirectory(${JAMBA_ROOT}/audio-unit/plugin auplugin)
  else ()
    message(STATUS "Audio Unit disabled (requires XCode CMake generator)")
  endif ()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_au_targets
#------------------------------------------------------------------------
function(internal_jamba_add_au_targets)
  if (XCODE)
    #------------------------------------------------------------------------
    # build_au target | can be changed by setting BUILD_AU_TARGET before calling this function
    #------------------------------------------------------------------------
    if (NOT BUILD_AU_TARGET)
      set(BUILD_AU_TARGET "${ARG_TARGET}_au")
    endif ()
    add_custom_target("${ARG_TARGETS_PREFIX}build_au" DEPENDS "${BUILD_AU_TARGET}")
    add_dependencies("${ARG_TARGETS_PREFIX}build_all" "${ARG_TARGETS_PREFIX}build_au")

    set(AU_PLUGIN_SRC $<TARGET_BUNDLE_DIR:${BUILD_AU_TARGET}>)
    set(AU_PLUGIN_DST_DIR "Components")
    set(AU_PLUGIN_EXTENSION "component")

    internal_jamba_create_install_target("au" "${AU_PLUGIN_SRC}" "${AU_PLUGIN_DST_DIR}" "${AU_PLUGIN_EXTENSION}")
  else ()
    # XCode is required to build audio unit => simply display a message about it
    add_custom_target("${ARG_TARGETS_PREFIX}build_au"
        COMMAND ${CMAKE_COMMAND} -E echo "You need to use the XCode generator to build the Audio Unit wrapper"
        )
    add_custom_target("${ARG_TARGETS_PREFIX}install_au"
        COMMAND ${CMAKE_COMMAND} -E echo "You need to use the XCode generator to build the Audio Unit wrapper"
        )
    add_custom_target("${ARG_TARGETS_PREFIX}uninstall_au"
        COMMAND ${CMAKE_COMMAND} -E echo "You need to use the XCode generator to build the Audio Unit wrapper"
        )
  endif ()

  #------------------------------------------------------------------------
  # add to install_all / uninstall_all
  #------------------------------------------------------------------------
  add_dependencies("${ARG_TARGETS_PREFIX}install_all" "${ARG_TARGETS_PREFIX}install_au")
  add_dependencies("${ARG_TARGETS_PREFIX}uninstall_all" "${ARG_TARGETS_PREFIX}uninstall_au")
endfunction()

