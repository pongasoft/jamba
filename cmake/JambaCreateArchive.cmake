#------------------------------------------------------------------------
# internal_jamba_create_archive - Create archive (.tgz)
#------------------------------------------------------------------------
function(internal_jamba_create_archive target plugin_name)
  if (MAC)
    set(ARCHITECTURE "macOS_64bits")
  elseif (WIN)
    set(ARCHITECTURE "win_64bits")
  endif ()

  set(JAMBA_RELEASE_FILENAME "${plugin_name}" PARENT_SCOPE)

  set(ARCHIVE_NAME ${target}-${ARCHITECTURE}-${PLUGIN_VERSION})
  set(ARCHIVE_PATH ${CMAKE_BINARY_DIR}/archive/${ARCHIVE_NAME})

  add_custom_command(OUTPUT ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/archive/README-${ARCHITECTURE}.txt ${ARCHIVE_PATH}/README.txt
      )

  if (MAC)
    if (XCODE)
      add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}.${VST3_EXTENSION} ${ARCHIVE_PATH}/${plugin_name}.vst3
          DEPENDS ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}.${VST3_EXTENSION}
          DEPENDS ${ARCHIVE_PATH}
          WORKING_DIRECTORY archive
          )
      if (JAMBA_ENABLE_AUDIO_UNIT)
        add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}_au.component ${ARCHIVE_PATH}/${plugin_name}.component
            DEPENDS ${VST3_OUTPUT_DIR}/$<CONFIG>/${target}_au.component
            DEPENDS ${ARCHIVE_PATH}
            WORKING_DIRECTORY archive
            APPEND
            )
      endif ()
    else ()
      add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${VST3_OUTPUT_DIR}/${target}.${VST3_EXTENSION} ${ARCHIVE_PATH}/${plugin_name}.vst3
          DEPENDS ${VST3_OUTPUT_DIR}/${target}.${VST3_EXTENSION}
          DEPENDS ${ARCHIVE_PATH}
          WORKING_DIRECTORY archive
          )
    endif ()
  elseif (WIN)
    add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${ARCHIVE_PATH}/${plugin_name}.vst3
        DEPENDS ${target}
        DEPENDS ${ARCHIVE_PATH}
        WORKING_DIRECTORY archive
        )
  endif ()

  add_custom_command(OUTPUT ${ARCHIVE_PATH}.zip
      COMMAND ${CMAKE_COMMAND} -E tar cvf ${ARCHIVE_NAME}$<$<CONFIG:Debug>:_Debug>.zip --format=zip ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${ARCHIVE_PATH}
      COMMAND ${CMAKE_COMMAND} -E echo "Archive available under ${CMAKE_BINARY_DIR}/archive/${ARCHIVE_NAME}$<$<CONFIG:Debug>:_Debug>.zip"
      APPEND
      )

  add_custom_target(archive
      DEPENDS ${ARCHIVE_PATH}.zip
      )
endfunction()
