#-------------------------------------------------------------------------------
# If including this module, it means that VST2 is required
#-------------------------------------------------------------------------------

if(NOT VST2_SDK_ROOT)
  if(MAC)
    set(VST2_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.24" CACHE PATH "Location of VST2 SDK")
  elseif(WIN)
    set(VST2_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.24" CACHE PATH "Location of VST2 SDK")
  else()
    set(VST2_SDK_ROOT "" CACHE PATH "Location of VST2 SDK")
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
    message(WARNING "VST2_SDK_ROOT should point to the SDK directly and has been adjusted to ${VST2_SDK_ROOT}")
  else()
    message(FATAL_ERROR "VST2_SDK_ROOT=${VST2_SDK_ROOT} does not seem to point to a valid VST2 SDK (VST2 is enabled)")
  endif()
endif()

message(STATUS "VST2_SDK_ROOT=${VST2_SDK_ROOT}")
