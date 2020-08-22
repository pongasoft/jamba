# Copyright (c) 2020 pongasoft
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

#------------------------------------------------------------------------
# If no local install and download is allowed => fetch it from github
#------------------------------------------------------------------------
if(NOT EXISTS "${VST3_SDK_ROOT}/${VSTSDK3_KNOWN_FILE}" AND JAMBA_DOWNLOAD_VSTSDK)
  include(FetchContent)

  set(FETCHCONTENT_SOURCE_DIR_VST3SDK "")

  FetchContent_Declare(vst3sdk
      GIT_REPOSITORY    ${JAMBA_VST3SDK_GIT_REPO}
      GIT_TAG           ${JAMBA_VST3SDK_GIT_TAG}
      GIT_CONFIG        advice.detachedHead=false
      GIT_PROGRESS      true
      SOURCE_DIR        "${CMAKE_BINARY_DIR}/vst3sdk"
      BINARY_DIR        "${CMAKE_BINARY_DIR}/vst3sdk-build"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND     ""
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      )

  FetchContent_GetProperties(vst3sdk)

  if(NOT vst3sdk_POPULATED)
    message(STATUS "Fetching vst3sdk ${VST3SDK_GIT_REPO}@${VST3SDK_GIT_TAG}")
    FetchContent_Populate(vst3sdk)
    set(VST3_SDK_ROOT "${vst3sdk_SOURCE_DIR}" CACHE PATH "Location of VST3 SDK" FORCE)
  endif()
endif()

#------------------------------------------------------------------------
# Final check => must find a valid VST3 installation!
#------------------------------------------------------------------------
if(NOT EXISTS "${VST3_SDK_ROOT}/${VSTSDK3_KNOWN_FILE}")
  message(FATAL_ERROR "Unable to find (local or remote) a valid VST3 install [VST3_SDK_ROOT=${VST3_SDK_ROOT}]")
endif()