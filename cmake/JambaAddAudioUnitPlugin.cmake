#------------------------------------------------------------------------
# This module will build the audio unit plugin wrapper
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# internal_add_au_plugin
#------------------------------------------------------------------------
function(internal_add_au_plugin)
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
# internal_add_au_targets
#------------------------------------------------------------------------
function(internal_add_au_targets)
  if (XCODE)
    #------------------------------------------------------------------------
    # build_au target | can be changed by setting BUILD_AU_TARGET before calling this function
    #------------------------------------------------------------------------
    if (NOT BUILD_AU_TARGET)
      set(BUILD_AU_TARGET "${ARG_TARGET}_au")
    endif ()
    add_custom_target("${ARG_TARGETS_PREFIX}build_au" DEPENDS "${BUILD_AU_TARGET}")

    set(AU_PLUGIN_SRC $<TARGET_BUNDLE_DIR:${BUILD_AU_TARGET}>)
    set(AU_PLUGIN_DST_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/Components")
    set(AU_PLUGIN_EXTENSION "component")

    internal_jamba_create_install_target("au" "${AU_PLUGIN_SRC}" "${AU_PLUGIN_DST_DIR}" "${AU_PLUGIN_EXTENSION}")

    find_program(PlistBuddy_EXE PlistBuddy "/usr/libexec")
    find_program(auvaltool_EXE auvaltool "/usr/bin")

    if((NOT PlistBuddy_EXE) OR (NOT auvaltool_EXE))
      add_custom_target("${ARG_TARGETS_PREFIX}validate_au"
          COMMAND ${CMAKE_COMMAND} -E echo "Could not find either /usr/libexec/PlistBuddy or /usr/bin/auvaltool required for validating the Audio Unit"
          )
    else()
      set(AU_PLIST_FILE "$<TARGET_BUNDLE_DIR:${BUILD_AU_TARGET}>/Contents/Info.plist")
      set(AUVALTOOL_ARGS_FILE "${CMAKE_CURRENT_BINARY_DIR}/auvaltool.args")
      add_custom_command(OUTPUT "${AUVALTOOL_ARGS_FILE}"
          COMMAND "${PlistBuddy_EXE}" -c "Print :AudioComponents:0:type" -c "Print :AudioComponents:0:subtype" -c "Print :AudioComponents:0:manufacturer" "${AU_PLIST_FILE}" > "${AUVALTOOL_ARGS_FILE}"
          DEPENDS ${ARG_TARGETS_PREFIX}install_au)
      add_custom_target("${ARG_TARGETS_PREFIX}validate_au"
          COMMAND cat "${AUVALTOOL_ARGS_FILE}" | xargs -t "${auvaltool_EXE}" -v
          DEPENDS "${AUVALTOOL_ARGS_FILE}"
          )
    endif()
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
    add_custom_target("${ARG_TARGETS_PREFIX}validate_au"
        COMMAND ${CMAKE_COMMAND} -E echo "You need to use the XCode generator to build the Audio Unit wrapper"
        )
  endif ()

  #------------------------------------------------------------------------
  # add to install_all / uninstall_all
  #------------------------------------------------------------------------
  add_dependencies("${ARG_TARGETS_PREFIX}install_all" "${ARG_TARGETS_PREFIX}install_au")
  add_dependencies("${ARG_TARGETS_PREFIX}uninstall_all" "${ARG_TARGETS_PREFIX}uninstall_au")
endfunction()

# invoke the functions
internal_add_au_plugin()
internal_add_au_targets()

###############################################################################
# Function Name : validate_au()
###############################################################################
#  validate_au()
#{
#if [ "${JAMBA_ENABLE_AUDIO_UNIT}" == "ON" ]; then
#install_au
#
#AU_BUILD_COMPONENT="VST3/${BUILD_CONFIG}/${BUILD_TARGET}_au.component"
#AU_PLIST_FILE="$AU_BUILD_COMPONENT/Contents/Info.plist"
#if [ -z "$DRY_RUN" ]; then
#AU_TYPE=`/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:type" "${AU_PLIST_FILE}"`
#AU_SUBTYPE=`/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:subtype" "${AU_PLIST_FILE}"`
#AU_MANUFACTURER=`/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:manufacturer" "${AU_PLIST_FILE}"`
#else
#${DRY_RUN} AU_TYPE=/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:type" "${AU_PLIST_FILE}"
#${DRY_RUN} AU_SUBTYPE=/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:subtype" "${AU_PLIST_FILE}"
#${DRY_RUN} AU_MANUFACTURER=/usr/libexec/PlistBuddy -c "Print :AudioComponents:0:manufacturer" "${AU_PLIST_FILE}"
#AU_TYPE="\$AU_TYPE"
#AU_SUBTYPE="\$AU_SUBTYPE"
#AU_MANUFACTURER="\$AU_MANUFACTURER"
#fi
#${DRY_RUN} /usr/bin/auvaltool -v $AU_TYPE $AU_SUBTYPE $AU_MANUFACTURER
#fi
#}
