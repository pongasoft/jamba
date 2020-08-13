function(jamba_add_vst3_plugin)

  #############################################
  # Argument parsing / default values
  #############################################
  set(options "")
  set(oneValueArgs TARGET UIDESC RELEASE_FILENAME TARGETS_PREFIX)
  set(multiValueArgs VST_SOURCES RESOURCES)
  cmake_parse_arguments(
      "ARG" # prefix
      "${options}" # options
      "${oneValueArgs}" # single values
      "${multiValueArgs}" # multiple values
      ${ARGN}
  )

  macro(set_default_value name default_value)
    if(NOT ${name})
      set(${name} ${default_value})
    endif()
  endmacro()

  # Make sure ARG_TARGET has a value (default to project name if not provided)
  set_default_value(ARG_TARGET ${CMAKE_PROJECT_NAME})
  set_default_value(ARG_RELEASE_FILENAME ${ARG_TARGET})

  message(STATUS "ARG_TARGET=${ARG_TARGET}")

  # Adds the VST3 plugin
  internal_jamba_add_vst3()

  # Add all resources (uidesc + graphics)
  internal_jamba_add_all_resources()

  # Adds VST2 (if enabled)
  internal_jamba_add_vst2()

  # Adds Audio Unit (if enabled)
  internal_jamba_add_audio_unit()

  # Adds jamba.sh / jamba.bat
  internal_jamba_add_dev_scripts()

  include(JambaCreateTargets)
  internal_jamba_create_targets()

endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_vst3
#------------------------------------------------------------------------
function(internal_jamba_add_vst3)
  smtg_add_vst3plugin(${ARG_TARGET} ${VST3_SDK_ROOT} ${ARG_VST_SOURCES})

  # We need to link with jamba (which defines what it needs to link with)
  target_link_libraries(${ARG_TARGET} PUBLIC jamba)

  if(XCODE)
    # $<TARGET_BUNDLE_DIR:tgt> relies on this property so we must set it (not set by VST3 SDK when XCODE)
    set_target_properties(${ARG_TARGET} PROPERTIES BUNDLE_EXTENSION "vst3")
  endif()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_vst2
#------------------------------------------------------------------------
function(internal_jamba_add_vst2)
  if (JAMBA_ENABLE_VST2)
    message(STATUS "${ARG_TARGET} will be VST2 compatible")
    if (MAC)
      # fix missing VSTPluginMain symbol when also building VST 2 version
      smtg_set_exported_symbols(${ARG_TARGET} "${JAMBA_ROOT}/mac/macexport_vst2.exp")
    endif ()
    if (WIN)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif ()
  endif ()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_audio_unit
#------------------------------------------------------------------------
function(internal_jamba_add_audio_unit)
  if (JAMBA_ENABLE_AUDIO_UNIT)
    if(XCODE)
      set(JAMBA_VST3_PLUGIN_TARGET "${ARG_TARGET}")
      # add the wrapper
      add_subdirectory(${JAMBA_ROOT}/audio-unit/auwrapper auwrapper)
      # add the plugin
      if (NOT AU_PLUGIN_ROOT)
        set(AU_PLUGIN_ROOT ${PROJECT_SOURCE_DIR}/audio-unit)
      endif ()
      add_subdirectory(${JAMBA_ROOT}/audio-unit/plugin auplugin)
    else()
      message(STATUS "Audio Unit disabled (requires XCode CMake generator)")
    endif()
  endif ()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_all_resources
#------------------------------------------------------------------------
function(internal_jamba_add_all_resources)
  set(JAMBA_VST3_RESOURCES_RC "")

  get_filename_component(UIDESC_FILENAME "${ARG_UIDESC}" NAME_WLE)

  internal_jamba_add_resource("${ARG_UIDESC}" "DATA" "" JAMBA_VST3_RESOURCES_RC)
  internal_jamba_add_resources("${JAMBA_VST3_RESOURCES_RC}" JAMBA_VST3_RESOURCES_RC)

  if (MAC)
    if (NOT JAMBA_VST3_PLUGIN_MAC_INFO_PLIST)
      set(JAMBA_VST3_PLUGIN_MAC_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/mac/Info.plist")
    endif ()
    smtg_set_bundle(${ARG_TARGET} INFOPLIST "${JAMBA_VST3_PLUGIN_MAC_INFO_PLIST}" PREPROCESS)
  elseif (WIN)
    list(JOIN JAMBA_VST3_RESOURCES_RC "\n" JAMBA_VST3_RESOURCES_RC)
    file(WRITE "${CMAKE_BINARY_DIR}/generated/vst3_resources.rc" ${JAMBA_VST3_RESOURCES_RC})
    target_sources(${ARG_TARGET} PRIVATE resource/${UIDESC_FILENAME}.rc)
  endif ()

endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_resources
#------------------------------------------------------------------------
function(internal_jamba_add_resources inList outList)
  foreach(resource IN LISTS ARG_RESOURCES)
    get_filename_component(type "${resource}" LAST_EXT)
    string(SUBSTRING "${type}" 1 -1 type)
    string(TOUPPER "${type}" type)
    internal_jamba_add_resource("${resource}" "${type}" "${inList}" inList)
  endforeach()
  set(${outList} "${inList}" PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_resources
#------------------------------------------------------------------------
function(internal_jamba_add_resource resource type inList outList)
  get_filename_component(filename "${resource}" NAME_WLE)
  smtg_add_vst3_resource(${ARG_TARGET} "${resource}" "")
  file(TO_NATIVE_PATH "${resource}" JAMBA_VST3_RESOURCE_PATH)
  string(REPLACE "\\" "\\\\" JAMBA_VST3_RESOURCE_PATH "${JAMBA_VST3_RESOURCE_PATH}")
  list(APPEND inList "${filename}\t${type}\t\"${JAMBA_VST3_RESOURCE_PATH}\"")
  set(${outList} ${inList} PARENT_SCOPE)
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_dev_scripts
#------------------------------------------------------------------------
function(internal_jamba_add_dev_scripts)
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
endfunction()
