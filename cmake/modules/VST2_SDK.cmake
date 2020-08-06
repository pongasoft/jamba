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

if(VST2_SDK_ROOT)
  MESSAGE(STATUS "VST2_SDK_ROOT=${VST2_SDK_ROOT}")
else()
  MESSAGE(FATAL_ERROR "You have enabled VST2 support but VST2_SDK_ROOT is not defined. Please use -DVST2_SDK_ROOT=<path to VST2 sdk>.")
endif()

include_directories(${VST2_SDK_ROOT})
