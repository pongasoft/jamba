#------------------------------------------------------------------------
# This module checks for a local install of the VST3 SDK either in a default location, or in the location
# provided by VST3_SDK_ROOT cache path variable.
# If a local install is not found and JAMBA_DOWNLOAD_VSTSDK is ON then it will fetch it from github
# Note that by default JAMBA_DOWNLOAD_VSTSDK is OFF because the download can be quite large so you must
# enable it explicitly.
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# Determine behavior when no local install
#------------------------------------------------------------------------
option(JAMBA_DOWNLOAD_VSTSDK "Download VST SDK if not installed locally" OFF)

#------------------------------------------------------------------------
# Determine local / default location
# Either install the SDK in its default location or provide VST3_SDK_ROOT
#------------------------------------------------------------------------
if(APPLE)
  set(VST3_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.${JAMBA_VST3SDK_VERSION}" CACHE PATH "Location of VST3 SDK")
elseif(WIN32)
  set(VST3_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.${JAMBA_VST3SDK_VERSION}" CACHE PATH "Location of VST3 SDK")
else()
  message(FATAL_ERROR "Jamba currently supports only macOS and Windows")
endif()

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
      GIT_SHALLOW       true
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