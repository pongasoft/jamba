#------------------------------------------------------------------------
# This module creates the archive (artifact to install the plugin)
# Must define jamba_create_archive
#------------------------------------------------------------------------
function(jamba_create_archive)
  if (MAC)
    set(ARCHITECTURE "macOS_64bits")
  elseif (WIN)
    set(ARCHITECTURE "win_64bits")
  endif ()

  set_default_value(CPACK_PACKAGE_NAME                "${ARG_RELEASE_FILENAME}")
  set_default_value(CPACK_PACKAGE_VERSION_MAJOR       "${PLUGIN_MAJOR_VERSION}")
  set_default_value(CPACK_PACKAGE_VERSION_MINOR       "${PLUGIN_MINOR_VERSION}")
  set_default_value(CPACK_PACKAGE_VERSION_PATCH       "${PLUGIN_PATCH_VERSION}")
  set_default_value(CPACK_SYSTEM_NAME                 "${ARCHITECTURE}")

  set(CPACK_GENERATOR ZIP)
  set(CPACK_VERBATIM_VARIABLES TRUE)
  set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
  set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

  # Add the doc folder at the root if it exists
  if(EXISTS "${ARG_ARCHIVE_DOC_DIR}")
    set(CPACK_INSTALLED_DIRECTORIES "${ARG_ARCHIVE_DOC_DIR}" ".")
  endif()

  include(CPack)

  cpack_add_component(vst3)

  if (JAMBA_ENABLE_VST2)
    cpack_add_component(vst2)
  endif()

  if (MAC AND JAMBA_ENABLE_AUDIO_UNIT AND XCODE)
    cpack_add_component(au)
  endif()

  add_custom_target("${ARG_TARGETS_PREFIX}create_archive"
      COMMAND ${CMAKE_COMMAND} -E echo "Creating archive..."
      COMMAND "${CMAKE_CPACK_COMMAND}" --verbose -G ZIP -C $<CONFIG>
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" # CPackConfig.cmake is created there
      DEPENDS "${ARG_TARGETS_PREFIX}build_all"
      )

endfunction()
