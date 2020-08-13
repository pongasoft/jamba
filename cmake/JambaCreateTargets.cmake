#------------------------------------------------------------------------
# internal_jamba_create_targets
#------------------------------------------------------------------------
function(internal_jamba_create_targets)
  #------------------------------------------------------------------------
  # build_vst3 target | can be changed by setting BUILD_VST3_TARGET before calling this function
  #------------------------------------------------------------------------
  if (NOT BUILD_VST3_TARGET)
    set(BUILD_VST3_TARGET "${ARG_TARGET}")
  endif ()
  add_custom_target("${ARG_TARGETS_PREFIX}build_vst3" DEPENDS "${BUILD_VST3_TARGET}")

  #------------------------------------------------------------------------
  # test_vst3 target | can be changed by setting TEST_VST3_TARGET before calling this function
  #------------------------------------------------------------------------
  if (NOT TEST_VST3_TARGET)
    set(TEST_VST3_TARGET "${ARG_TARGET}_test")
  endif ()
  add_custom_target("${ARG_TARGETS_PREFIX}test_vst3" DEPENDS "${TEST_VST3_TARGET}")

  # add install targets so that they can be invoked by the scripts (and from the IDE)
  if (MAC)
    set(VST3_PLUGIN_SRC $<TARGET_BUNDLE_DIR:${ARG_TARGET}>)
    set(VST3_PLUGIN_DST_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3")
    set(VST3_PLUGIN_EXTENSION "vst3")
    set(VST2_PLUGIN_DST_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST")
    set(VST2_PLUGIN_EXTENSION "vst")
  elseif(WIN)
    set(VST3_PLUGIN_SRC $<TARGET_FILE:${ARG_TARGET}>)
    set(VST3_PLUGIN_DST_DIR "C:/Program\ Files/Common\ Files/VST3")
    set(VST3_PLUGIN_EXTENSION "vst3")
    set(VST2_PLUGIN_DST_DIR "C:/Program\ Files/VSTPlugins")
    set(VST2_PLUGIN_EXTENSION "dll")
  endif()

  #------------------------------------------------------------------------
  # install_vst3 target
  # uninstall_vst3 target
  # install_vst target (for installing both vst2 and vst3)
  # uninstall target (for uninstalling all)
  #------------------------------------------------------------------------
  internal_jamba_create_install_target("vst3" ${VST3_PLUGIN_SRC} ${VST3_PLUGIN_DST_DIR} ${VST3_PLUGIN_EXTENSION})
  add_custom_target("${ARG_TARGETS_PREFIX}install_vst" DEPENDS "${ARG_TARGETS_PREFIX}install_vst3")
  add_custom_target("${ARG_TARGETS_PREFIX}uninstall" DEPENDS "${ARG_TARGETS_PREFIX}uninstall_vst3")

  #------------------------------------------------------------------------
  # install_vst2 target
  # uninstall_vst2 target
  #------------------------------------------------------------------------
  if (JAMBA_ENABLE_VST2)
    add_custom_target("${ARG_TARGETS_PREFIX}build_vst2" DEPENDS "${ARG_TARGETS_PREFIX}build_vst3")
    internal_jamba_create_install_target("vst2" ${VST3_PLUGIN_SRC} ${VST2_PLUGIN_DST_DIR} ${VST2_PLUGIN_EXTENSION})
    add_dependencies("${ARG_TARGETS_PREFIX}install_vst" "${ARG_TARGETS_PREFIX}install_vst2")
    add_dependencies("${ARG_TARGETS_PREFIX}uninstall" "${ARG_TARGETS_PREFIX}uninstall_vst2")
  endif()

  if (MAC AND JAMBA_ENABLE_AUDIO_UNIT)
    if(XCODE)
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

      internal_jamba_create_install_target("au" ${AU_PLUGIN_SRC} ${AU_PLUGIN_DST_DIR} ${AU_PLUGIN_EXTENSION})
      add_dependencies("${ARG_TARGETS_PREFIX}uninstall" "${ARG_TARGETS_PREFIX}uninstall_au")
    else()
      # XCode is required to build audio unit => simply display a message about it
      add_custom_target("${ARG_TARGETS_PREFIX}build_au"
          COMMAND ${CMAKE_COMMAND} -E echo "You need to use the XCode generator to build the Audio Unit wrapper"
          )
      add_custom_target("${ARG_TARGETS_PREFIX}install_au" DEPENDS "${ARG_TARGETS_PREFIX}build_au")
    endif()
  endif()

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

endfunction()

#------------------------------------------------------------------------
# internal_jamba_create_install_target
#------------------------------------------------------------------------
function(internal_jamba_create_install_target component src dstDir extension)
  if(MAC)
    # Cannot use generator expression in install => must define multiple with different CONFIGURATIONS
    install(DIRECTORY "${src}/"
        COMPONENT ${component}
        CONFIGURATIONS Debug
        DESTINATION "${dstDir}/${ARG_RELEASE_FILENAME}_Debug.${extension}"
        )
    install(DIRECTORY "${src}/"
        COMPONENT ${component}
        CONFIGURATIONS Release
        DESTINATION "${dstDir}/${ARG_RELEASE_FILENAME}.${extension}"
        )
  elseif(WIN)
    install(FILES ${src}
        COMPONENT ${component}
        DESTINATION ${dstDir}
        CONFIGURATIONS Debug
        RENAME "${ARG_RELEASE_FILENAME}_Debug.${extension}"
        )
    install(FILES ${src}
        COMPONENT ${component}
        DESTINATION ${dstDir}
        CONFIGURATIONS Release
        RENAME "${ARG_RELEASE_FILENAME}.${extension}"
        )
  endif()

  set(PLUGIN_FILENAME "${ARG_RELEASE_FILENAME}$<$<CONFIG:Debug>:_Debug>.${extension}")

  add_custom_target("${ARG_TARGETS_PREFIX}install_${component}"
      COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --component ${component} --config $<CONFIG>
      DEPENDS "${ARG_TARGETS_PREFIX}build_${component}"
      COMMAND ${CMAKE_COMMAND} -E echo "Installed ${component} plugin under ${dstDir}/${PLUGIN_FILENAME}"
      )

  add_custom_target("${ARG_TARGETS_PREFIX}uninstall_${component}"
      COMMAND ${CMAKE_COMMAND} -E rm -r -f "${dstDir}/${PLUGIN_FILENAME}"
      COMMAND ${CMAKE_COMMAND} -E echo "Removed ${component} plugin [${PLUGIN_FILENAME}] from ${dstDir}"
      )

endfunction()
