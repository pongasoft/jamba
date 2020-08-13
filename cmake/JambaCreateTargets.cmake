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
  set(PLUGIN_FILENAME ${ARG_RELEASE_FILENAME}$<$<CONFIG:Debug>:_Debug>)

  if (MAC)
    set(VST3_PLUGIN_SRC $<TARGET_BUNDLE_DIR:${ARG_TARGET}>)
    set(VST3_PLUGIN_DST_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3")
    set(VST3_PLUGIN_DST_NAME "${PLUGIN_FILENAME}.vst3")
    set(VST2_PLUGIN_DST_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST")
    set(VST2_PLUGIN_DST_NAME "${PLUGIN_FILENAME}.vst")
  elseif(WIN)
    set(VST3_PLUGIN_SRC $<TARGET_FILE:${ARG_TARGET}>)
    set(VST3_PLUGIN_DST_DIR "C:/Program\ Files/Common\ Files/VST3")
    set(VST3_PLUGIN_DST_NAME "${PLUGIN_FILENAME}.vst3")
    set(VST2_PLUGIN_DST_DIR "C:/Program\ Files/VSTPlugins")
    set(VST2_PLUGIN_DST_NAME "${PLUGIN_FILENAME}.dll")
  endif()

  #------------------------------------------------------------------------
  # install_vst3 target
  # uninstall_vst3 target
  # install_vst target (for installing both vst2 and vst3)
  # uninstall target (for uninstalling all)
  #------------------------------------------------------------------------
  internal_jamba_create_install_target("vst3" ${VST3_PLUGIN_SRC} ${VST3_PLUGIN_DST_DIR} ${VST3_PLUGIN_DST_NAME})
  add_custom_target("${ARG_TARGETS_PREFIX}install_vst" DEPENDS "${ARG_TARGETS_PREFIX}install_vst3")
  add_custom_target("${ARG_TARGETS_PREFIX}uninstall" DEPENDS "${ARG_TARGETS_PREFIX}uninstall_vst3")

  #------------------------------------------------------------------------
  # install_vst2 target
  # uninstall_vst2 target
  #------------------------------------------------------------------------
  if (JAMBA_ENABLE_VST2)
    add_custom_target("${ARG_TARGETS_PREFIX}build_vst2" DEPENDS "${ARG_TARGETS_PREFIX}build_vst3")
    internal_jamba_create_install_target("vst2" ${VST3_PLUGIN_SRC} ${VST2_PLUGIN_DST_DIR} ${VST2_PLUGIN_DST_NAME})
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
      set(AU_PLUGIN_DST_NAME "${PLUGIN_FILENAME}.component")

      internal_jamba_create_install_target("au" ${AU_PLUGIN_SRC} ${AU_PLUGIN_DST_DIR} ${AU_PLUGIN_DST_NAME})
      add_dependencies("${ARG_TARGETS_PREFIX}uninstall" "${ARG_TARGETS_PREFIX}uninstall_au")
    else()
      # XCode is required to build audio unit => simply display a message about it
      add_custom_target("${ARG_TARGETS_PREFIX}build_au"
          COMMAND ${CMAKE_COMMAND} -E echo "You need to use the XCode generator to build the Audio Unit wrapper"
          )
      add_custom_target("${ARG_TARGETS_PREFIX}install_au" DEPENDS "${ARG_TARGETS_PREFIX}build_au")
    endif()
  endif()

  ## set(VST3_PLUGIN_EXE $<TARGET_FILE:${ARG_TARGET}>)

endfunction()

#------------------------------------------------------------------------
# internal_jamba_create_install_target
#------------------------------------------------------------------------
function(internal_jamba_create_install_target component src dstDir dstName)
  if(MAC)
    install(DIRECTORY "${src}/"
        COMPONENT ${component}
        DESTINATION "${dstDir}/${dstName}")
  elseif(WIN)
    install(FILES ${src}
        COMPONENT ${component}
        DESTINATION ${dstDir}
        RENAME ${dstName}
        )
  endif()

  add_custom_target("${ARG_TARGETS_PREFIX}install_${component}"
      COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --component ${component} --config $<CONFIG>
      DEPENDS "${ARG_TARGETS_PREFIX}build_${component}"
      COMMAND ${CMAKE_COMMAND} -E echo "Installed ${component} plugin under ${dstDir}/${dstName}"
      )

  add_custom_target("${ARG_TARGETS_PREFIX}uninstall_${component}"
      COMMAND ${CMAKE_COMMAND} -E rm -r -f "${dstDir}/${dstName}"
      COMMAND ${CMAKE_COMMAND} -E echo "Removed ${component} plugin [${dstName}] from ${dstDir}"
      )

endfunction()
