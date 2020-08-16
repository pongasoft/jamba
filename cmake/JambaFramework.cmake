#------------------------------------------------------------------------
# This module defines the Jamba Framework which contains the main
# jamba_add_vst3_plugin function to be called by the plugin CMakeFile.txt
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# internal_jamba_create_install_target
#------------------------------------------------------------------------
function(internal_jamba_create_install_target component src dstDir extension)
  if(MAC)
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
  elseif(WIN)
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

  set(PLUGIN_FILENAME "${ARG_RELEASE_FILENAME}$<$<CONFIG:Debug>:_Debug>.${extension}")

  #------------------------------------------------------------------------
  # target install_<component>
  # Invokes cmake --install with right component and config
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}install_${component}"
      COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --component "${component}" --config $<CONFIG>
      DEPENDS "${ARG_TARGETS_PREFIX}build_${component}"
      COMMAND ${CMAKE_COMMAND} -E echo "Installed ${component} plugin under ${dstDir}/${PLUGIN_FILENAME}"
      )

  #------------------------------------------------------------------------
  # target uninstall_<component>
  # Deletes the folder / file installed with install_<component>
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}uninstall_${component}"
      COMMAND ${CMAKE_COMMAND} -E rm -r -f "${dstDir}/${PLUGIN_FILENAME}"
      COMMAND ${CMAKE_COMMAND} -E echo "Removed ${component} plugin [${PLUGIN_FILENAME}] from ${dstDir}"
      )
endfunction()

#------------------------------------------------------------------------
# jamba_add_vst3_plugin
#------------------------------------------------------------------------
function(jamba_add_vst3_plugin)
  #------------------------------------------------------------------------
  # Argument parsing / default values
  #------------------------------------------------------------------------
  set(options "")
  set(oneValueArgs TARGET TEST_TARGET UIDESC RELEASE_FILENAME TARGETS_PREFIX PYTHON3_EXECUTABLE)
  set(multiValueArgs VST_SOURCES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS COMPILE_OPTIONS LINK_LIBRARIES
                     RESOURCES
                     TEST_CASE_SOURCES TEST_SOURCES TEST_INCLUDE_DIRECTORIES TEST_COMPILE_DEFINITIONS TEST_COMPILE_OPTIONS TEST_LINK_LIBRARIES)
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
  set_default_value(ARG_TARGET "${CMAKE_PROJECT_NAME}")
  set_default_value(ARG_TEST_TARGET "${ARG_TARGET}_test")
  set_default_value(ARG_RELEASE_FILENAME "${ARG_TARGET}")

  # Adds the VST3 plugin
  include(JambaAddVST3Plugin)

  # Add all resources (uidesc + graphics)
  include(JambaAddAllResources)

  # Adds VST2 (if enabled)
  if (JAMBA_ENABLE_VST2)
    include(JambaAddVST2Plugin)
  endif()

  # Adds Audio Unit (if enabled)
  if (MAC AND JAMBA_ENABLE_AUDIO_UNIT)
    include(JambaAddAudioUnitPlugin)
  endif()

  # Adds jamba.sh / jamba.bat / jamba.py
  if(JAMBA_ENABLE_DEV_SCRIPT)
    include(JambaDevScript)
  endif()

  # Optionally setup testing
  if(JAMBA_ENABLE_TESTING)
    include(JambaAddTest)
  endif()

endfunction()
