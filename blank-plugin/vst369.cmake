cmake_minimum_required(VERSION 3.12)

if(NOT VST3_SDK_ROOT)
  if(APPLE)
    set(DEFAULT_VST3_SDK_ROOT "/Users/Shared/Steinberg/VST_SDK.369/VST3_SDK" CACHE PATH "Location of VST3 SDK")
  elseif(WIN32)
    set(DEFAULT_VST3_SDK_ROOT "C:/Users/Public/Documents/Steinberg/VST_SDK.369/VST3_SDK" CACHE PATH "Location of VST3 SDK")
  else()
    set(DEFAULT_VST3_SDK_ROOT "" CACHE PATH "Location of VST3 SDK")
  endif()

  if(NOT EXISTS "${DEFAULT_VST3_SDK_ROOT}")
    include(FetchContent)

    FetchContent_Declare(VST369
        PREFIX           "${CMAKE_BINARY_DIR}/vst3.6.9"
        URL              https://github.com/pongasoft/vst3sdk/releases/download/vst369_01_03_2018_build132/vstsdk369_01_03_2018_build_132.zip
        URL_HASH         "SHA256=7c6c2a5f0bcbf8a7a0d6a42b782f0d3c00ec8eafa4226bbf2f5554e8cd764964"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
        )

    FetchContent_GetProperties(VST369)

    if(NOT VST369_POPULATED)

      if(FETCHCONTENT_SOURCE_DIR_VST369)
        message(STATUS "Using VST369 from local ${FETCHCONTENT_SOURCE_DIR_VST369}")
      else()
        message(STATUS "Downloading VST369...")
      endif()

      FetchContent_Populate(VST369)

      # Copy vst2 to vst3
      if(JAMBA_ENABLE_VST2)
        file(COPY "${vst369_SOURCE_DIR}/VST2_SDK/" DESTINATION "${vst369_SOURCE_DIR}/VST3_SDK")
      endif()

      # Setting VST3_SDK_ROOT to the downloaded version
      set(VST3_SDK_ROOT "${vst369_SOURCE_DIR}/VST3_SDK" CACHE PATH "Location of VST3 SDK")
      message(STATUS "VST3_SDK_ROOT set to ${VST3_SDK_ROOT}")
    endif()
  else()
    set(VST3_SDK_ROOT "${DEFAULT_VST3_SDK_ROOT}" CACHE PATH "Location of VST3 SDK")
    message(STATUS "Detected default VST3.6.9 installation")
  endif()

endif()

