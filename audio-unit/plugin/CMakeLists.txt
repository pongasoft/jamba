if (MAC AND XCODE)
  set(target "${JAMBA_VST3_PLUGIN_TARGET}_au")
  set(${target}_sources
      ${AU_PLUGIN_ROOT}/audiounitconfig.h
      ${AU_PLUGIN_ROOT}/Info.plist
      )
  include_directories(${CMAKE_BINARY_DIR}/generated/)
  add_library(${target} MODULE ${${target}_sources})
  smtg_target_setup_universal_binary(${target})
  set_target_properties(${target} PROPERTIES BUNDLE TRUE)
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GENERATE_MASTER_OBJECT_FILE "YES")
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS "-all_load")
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE "YES")
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_WRAPPER_EXTENSION "component")
  set_target_properties(${target} PROPERTIES BUNDLE_EXTENSION "component") # for $<TARGET_BUNDLE_DIR:tgt>
  set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${VST3_OUTPUT_DIR})
  target_link_libraries(${target} PRIVATE jmb_auwrapper)
  smtg_target_set_bundle(${target} INFOPLIST "${AU_PLUGIN_ROOT}/Info.plist" PREPROCESS)
  set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_INFOPLIST_OTHER_PREPROCESSOR_FLAGS "-I ${CMAKE_BINARY_DIR}/generated")

  set(AU_OUTPUT_DIR ${VST3_OUTPUT_DIR}/$<CONFIG>)

  add_dependencies(${target} ${JAMBA_VST3_PLUGIN_TARGET})
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${AU_OUTPUT_DIR}/${target}.component/Contents/Resources)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${AU_OUTPUT_DIR}/${JAMBA_VST3_PLUGIN_TARGET}.vst3" "${AU_OUTPUT_DIR}/${target}.component/Contents/Resources/plugin.vst3")
endif ()
