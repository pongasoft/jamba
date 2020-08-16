#------------------------------------------------------------------------
# This module adds all the resources (images and .uidesc and .rc to the build)
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# internal_jamba_add_all_resources
#------------------------------------------------------------------------
function(internal_jamba_add_all_resources)
  set(JAMBA_VST3_RESOURCES_RC "")

  get_filename_component(UIDESC_FILENAME ${ARG_UIDESC} NAME_WLE)
  get_filename_component(UIDESC_DIR ${ARG_UIDESC} DIRECTORY)

  internal_jamba_add_resource("${ARG_UIDESC}" "DATA" "" JAMBA_VST3_RESOURCES_RC)
  internal_jamba_add_resources("${JAMBA_VST3_RESOURCES_RC}" JAMBA_VST3_RESOURCES_RC)

  if (MAC)
    smtg_set_bundle(${ARG_TARGET} INFOPLIST "${ARG_MAC_INFO_PLIST}" PREPROCESS)
  elseif (WIN)
    list(JOIN JAMBA_VST3_RESOURCES_RC "\n" JAMBA_VST3_RESOURCES_RC)
    file(WRITE "${CMAKE_BINARY_DIR}/generated/vst3_resources.rc" ${JAMBA_VST3_RESOURCES_RC})
    target_sources(${ARG_TARGET} PRIVATE "${UIDESC_DIR}/${UIDESC_FILENAME}.rc")
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
  get_filename_component(filename "${resource}" NAME)
  smtg_add_vst3_resource(${ARG_TARGET} "${resource}" "")
  file(TO_NATIVE_PATH "${resource}" JAMBA_VST3_RESOURCE_PATH)
  string(REPLACE "\\" "\\\\" JAMBA_VST3_RESOURCE_PATH "${JAMBA_VST3_RESOURCE_PATH}")
  list(APPEND inList "${filename}\t${type}\t\"${JAMBA_VST3_RESOURCE_PATH}\"")
  set(${outList} ${inList} PARENT_SCOPE)
endfunction()

# invoke the functions
internal_jamba_add_all_resources()