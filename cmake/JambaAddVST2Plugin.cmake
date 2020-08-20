#------------------------------------------------------------------------
# This module makes the VST3 plugin also VST2 compatible
# Must define jamba_add_vst2_plugin
#------------------------------------------------------------------------
function(jamba_add_vst2_plugin)
  internal_jamba_add_vst2_plugin()
  internal_jamba_add_vst2_targets()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_vst2_plugin
#------------------------------------------------------------------------
function(internal_jamba_add_vst2_plugin)
  message(STATUS "${ARG_TARGET} will be VST2 compatible")
  if (MAC)
    # fix missing VSTPluginMain symbol when also building VST 2 version
    smtg_set_exported_symbols(${ARG_TARGET} "${JAMBA_ROOT}/mac/macexport_vst2.exp")
  endif ()
endfunction()

#------------------------------------------------------------------------
# internal_jamba_add_vst2_targets
#------------------------------------------------------------------------
function(internal_jamba_add_vst2_targets)
  # add install targets so that they can be invoked by the scripts (and from the IDE)
  if (MAC)
    set(VST2_PLUGIN_SRC $<TARGET_BUNDLE_DIR:${ARG_TARGET}>)
    set(VST2_PLUGIN_DST_DIR "VST")
    set(VST2_PLUGIN_EXTENSION "vst")
  elseif (WIN)
    set(VST2_PLUGIN_SRC $<TARGET_FILE:${ARG_TARGET}>)
    set(VST2_PLUGIN_DST_DIR "VSTPlugins")
    set(VST2_PLUGIN_EXTENSION "dll")
  endif ()

  #------------------------------------------------------------------------
  # build_vst2 target (simply depends on build_vst3 but needed by internal_jamba_create_install_target)
  # install_vst2 target
  # uninstall_vst2 target
  #------------------------------------------------------------------------
  add_custom_target("${ARG_TARGETS_PREFIX}build_vst2" DEPENDS "${ARG_TARGETS_PREFIX}build_vst3")
  add_dependencies("${ARG_TARGETS_PREFIX}build_all" "${ARG_TARGETS_PREFIX}build_vst2")
  internal_jamba_create_install_target("vst2" "${VST2_PLUGIN_SRC}" "${VST2_PLUGIN_DST_DIR}" "${VST2_PLUGIN_EXTENSION}")

  #------------------------------------------------------------------------
  # add to install_vst / install_all / uninstall_all
  #------------------------------------------------------------------------
  add_dependencies("${ARG_TARGETS_PREFIX}install_vst" "${ARG_TARGETS_PREFIX}install_vst2")
  add_dependencies("${ARG_TARGETS_PREFIX}install_all" "${ARG_TARGETS_PREFIX}install_vst2")
  add_dependencies("${ARG_TARGETS_PREFIX}uninstall_all" "${ARG_TARGETS_PREFIX}uninstall_vst2")
endfunction()
