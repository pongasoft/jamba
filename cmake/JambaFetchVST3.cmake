# Copyright (c) 2020-2023 pongasoft
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# @author Yan Pujante

#------------------------------------------------------------------------
# This module checks for a local install of the VST3 SDK either in a default location, or in the location
# provided by VST3_SDK_ROOT cache path variable.
# If a local install is not found and JAMBA_DOWNLOAD_VSTSDK is ON then it will fetch it from github
# Note that by default JAMBA_DOWNLOAD_VSTSDK is OFF because the download can be quite large so you must
# enable it explicitly.
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# Check for valid installation
#------------------------------------------------------------------------
# This is the file that contains the version number... so should exist!
set(VSTSDK3_KNOWN_FILE "pluginterfaces/vst/vsttypes.h")

# we adjust the root if VST3_SDK_ROOT points to the root of the zip file instead of the embedded VST3_SDK folder
if(NOT EXISTS "${VST3_SDK_ROOT}/${VSTSDK3_KNOWN_FILE}" AND EXISTS "${VST3_SDK_ROOT}/VST3_SDK/${VSTSDK3_KNOWN_FILE}")
  set(VST3_SDK_ROOT "${VST3_SDK_ROOT}/VST3_SDK" CACHE PATH "Location of VST3 SDK" FORCE)
endif()

# the list of files that needs to be copied
set(INTERNAL_JAMBA_VST3SDK_PATCH_PATH_LIST CMakeLists.txt base cmake pluginterfaces public.sdk vstgui4)

#------------------------------------------------------------------------
# internal_jamba_copy_vst3sdk
#------------------------------------------------------------------------
macro(internal_jamba_copy_vst3sdk FROM_DIR TO_DIR)
  if(NOT INTERNAL_JAMBA_VST3SDK_COPY_LOCATION)
    message(STATUS "Copying: ${FROM_DIR} -> ${TO_DIR}")
    foreach(PATH IN LISTS INTERNAL_JAMBA_VST3SDK_PATCH_PATH_LIST)
      if(EXISTS "${FROM_DIR}/${PATH}")
        file(COPY "${FROM_DIR}/${PATH}" DESTINATION "${TO_DIR}")
      endif()
    endforeach()
    file(CHMOD_RECURSE "${TO_DIR}"
         FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
         DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_EXECUTE
        )
    set(INTERNAL_JAMBA_VST3SDK_COPY_LOCATION "${TO_DIR}" CACHE PATH "Location (copy) of VST3 SDK" FORCE)
  endif()
endmacro()

#------------------------------------------------------------------------
# internal_jamba_patch_vst3sdk
#------------------------------------------------------------------------
macro(internal_jamba_patch_vst3sdk FROM_DIR TO_DIR)
  if(NOT INTERNAL_JAMBA_VST3SDK_PATCH_LOCATION)
    message(STATUS "Patching: ${FROM_DIR} -> ${TO_DIR}")
    foreach(PATH IN LISTS INTERNAL_JAMBA_VST3SDK_PATCH_PATH_LIST)
      if(EXISTS "${FROM_DIR}/${PATH}")
        file(COPY "${FROM_DIR}/${PATH}" DESTINATION "${TO_DIR}")
      endif()
    endforeach()
    set(INTERNAL_JAMBA_VST3SDK_PATCH_LOCATION "${TO_DIR}" CACHE PATH "Location (patch) of VST3 SDK" FORCE)
  endif()
endmacro()

#------------------------------------------------------------------------
# If no local install and download is allowed => fetch it from github
#------------------------------------------------------------------------
if(EXISTS "${VST3_SDK_ROOT}/${VSTSDK3_KNOWN_FILE}")
  if(EXISTS "${JAMBA_VST3SDK_PATCH_DIR}")
    set(vst3sdk_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/vst3sdk-src")
    internal_jamba_copy_vst3sdk("${VST3_SDK_ROOT}" "${vst3sdk_SOURCE_DIR}")
    internal_jamba_patch_vst3sdk("${JAMBA_VST3SDK_PATCH_DIR}" "${vst3sdk_SOURCE_DIR}")
  else()
    # when no patching, there is no reason to copy the sdk...
    message(STATUS "No SDK patching")
    set(vst3sdk_SOURCE_DIR "${VST3_SDK_ROOT}")
  endif()
elseif(JAMBA_DOWNLOAD_VSTSDK)
  jamba_fetch_content(NAME vst3sdk GIT_REPO "${JAMBA_VST3SDK_GIT_REPO}" GIT_TAG "${JAMBA_VST3SDK_GIT_TAG}")
  if(EXISTS "${JAMBA_VST3SDK_PATCH_DIR}")
    internal_jamba_patch_vst3sdk("${JAMBA_VST3SDK_PATCH_DIR}" "${vst3sdk_SOURCE_DIR}")
  else()
    message(STATUS "No SDK patching")
  endif()
endif()

#------------------------------------------------------------------------
# Final check => must find a valid VST3 installation!
#------------------------------------------------------------------------
if(NOT EXISTS "${vst3sdk_SOURCE_DIR}/${VSTSDK3_KNOWN_FILE}")
  message(FATAL_ERROR "Unable to find (local or remote) a valid VST3 install [VST3_SDK_ROOT=${VST3_SDK_ROOT}]")
endif()