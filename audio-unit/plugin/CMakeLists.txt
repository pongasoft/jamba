if (MAC AND XCODE)
  set(target "${JAMBA_VST3_PLUGIN_TARGET}_au")
  set(${target}_sources
      ${AU_PLUGIN_ROOT}/audiounitconfig.h
      ${AU_PLUGIN_ROOT}/Info.plist
      )
  include_directories(${CMAKE_BINARY_DIR}/generated/)
  add_library(${target} MODULE ${${target}_sources})
  smtg_setup_universal_binary(${target})
  set_target_properties(${target} PROPERTIES BUNDLE TRUE)
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GENERATE_MASTER_OBJECT_FILE "YES")
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS "-all_load")
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE "YES")
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_WRAPPER_EXTENSION "component")
  set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${VST3_OUTPUT_DIR})
  target_link_libraries(${target} PRIVATE auwrapper)
  smtg_set_bundle(${target} INFOPLIST "${AU_PLUGIN_ROOT}/Info.plist" PREPROCESS)
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_OTHER_PREPROCESSOR_FLAGS "-I ${CMAKE_BINARY_DIR}/generated")

  set(AU_OUTPUT_DIR ${VST3_OUTPUT_DIR}/$<CONFIG>)

  add_dependencies(${target} ${JAMBA_VST3_PLUGIN_TARGET})
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${AU_OUTPUT_DIR}/${target}.component/Contents/Resources)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${AU_OUTPUT_DIR}/${JAMBA_VST3_PLUGIN_TARGET}.vst3" "${AU_OUTPUT_DIR}/${target}.component/Contents/Resources/plugin.vst3")

  execute_process(COMMAND xcrun --find Rez OUTPUT_VARIABLE OSX_REZ_COMMAND OUTPUT_STRIP_TRAILING_WHITESPACE)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND "${OSX_REZ_COMMAND}"
      "-d" "SystemSevenOrLater=1"
      "-script" "Roman"
      "-d" "i386_YES"
      "-d" "x86_64_YES"
      "-is" "${CMAKE_OSX_SYSROOT}"
      "-I" "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Versions/A/Headers"
      "-I" "/System/Library/Frameworks/CoreServices.framework/Frameworks/CarbonCore.framework/Versions/A/Headers"
      "-I" "/System/Library/Frameworks/AudioUnit.framework/Versions/A/Headers/"
      "-I" "${SMTG_COREAUDIO_SDK_PATH}/AudioUnits/AUPublic/AUBase"
      "-I" "${AU_PLUGIN_ROOT}"
      "-I" "${CMAKE_BINARY_DIR}/generated"
      "-o" "${AU_OUTPUT_DIR}/${target}.component/Contents/Resources/${target}.rsrc"
      "-useDF"
      "${VST3_SDK_ROOT}/public.sdk/source/vst/auwrapper/auresource.r"
      )
endif ()
