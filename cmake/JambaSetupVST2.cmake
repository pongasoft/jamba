#------------------------------------------------------------------------
# Determine local / default location
# Either install the SDK in its default location or provide VST2_SDK_ROOT
#------------------------------------------------------------------------
if(NOT VST2_SDK_ROOT)
  if(MAC)
    set(VST2_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.${JAMBA_VST2SDK_VERSION}" CACHE PATH "Location of VST2 SDK")
  elseif(WIN)
    set(VST2_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.${JAMBA_VST2SDK_VERSION}" CACHE PATH "Location of VST2 SDK")
  else()
    message(FATAL_ERROR "Jamba currently supports only macOS and Windows")
  endif()
endif()

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

# This is the main compiled file so should exist
set(VSTSDK2_KNOWN_FILE "public.sdk/source/vst2.x/audioeffect.cpp")

if(NOT EXISTS "${VST2_SDK_ROOT}/${VSTSDK2_KNOWN_FILE}")
  if(EXISTS "${VST2_SDK_ROOT}/VST2_SDK/${VSTSDK2_KNOWN_FILE}")
    set(VST2_SDK_ROOT "${VST2_SDK_ROOT}/VST2_SDK" CACHE PATH "Location of VST2 SDK" FORCE)
  else()
    message(FATAL_ERROR "VST2_SDK_ROOT=${VST2_SDK_ROOT} does not seem to point to a valid VST2 SDK (VST2 is enabled)")
  endif()
endif()

message(STATUS "VST2_SDK_ROOT=${VST2_SDK_ROOT}")
