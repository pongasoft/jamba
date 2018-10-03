cmake_minimum_required(VERSION 3.12)

if (MAC)
  if (XCODE)
    set(AU_WRAPPER_ROOT "${VST3_SDK_ROOT}/public.sdk/source/vst/auwrapper")
    set(target auwrapper)
    set(auwrapper_sources
        ${AU_WRAPPER_ROOT}/aucarbonview.mm
        ${AU_WRAPPER_ROOT}/aucarbonview.h
        ${AU_WRAPPER_ROOT}/aucocoaview.mm
        ${AU_WRAPPER_ROOT}/aucocoaview.h
        ${AU_WRAPPER_ROOT}/ausdk.mm
        ${AU_WRAPPER_ROOT}/auwrapper.mm
        ${AU_WRAPPER_ROOT}/auwrapper.h
        ${AU_WRAPPER_ROOT}/NSDataIBStream.mm
        ${AU_WRAPPER_ROOT}/NSDataIBStream.h
        ${VST3_SDK_ROOT}/pluginterfaces/base/funknown.cpp
        ${VST3_SDK_ROOT}/pluginterfaces/base/ustring.cpp
        ${VST3_SDK_ROOT}/pluginterfaces/base/coreiids.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/eventlist.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/eventlist.h
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostclasses.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostclasses.h
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/parameterchanges.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/parameterchanges.h
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/processdata.cpp
        ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/processdata.h
        )
    add_library(${target} STATIC ${auwrapper_sources})
    set_target_properties(${target} PROPERTIES ${SDK_IDE_LIBS_FOLDER})
    target_link_libraries(${target} PRIVATE base "-framework AudioUnit" "-framework CoreMIDI" "-framework AudioToolbox" "-framework CoreFoundation" "-framework Carbon" "-framework Cocoa" "-framework CoreAudio")
    target_include_directories(${target} PRIVATE "${SMTG_COREAUDIO_SDK_PATH}/**")
    string(MD5 AU_NAMESPACE_SUFFIX "${JAMBA_VST3_PLUGIN_TARGET}")
    file(WRITE "${CMAKE_BINARY_DIR}/generated/aucocoaclassprefix.h" "#define SMTG_AU_NAMESPACE SMTGAUCocoa${AU_NAMESPACE_SUFFIX}_\n")
    target_include_directories(${target} PRIVATE "${CMAKE_BINARY_DIR}/generated")
  else ()
    message("* To enable building the AudioUnit wrapper, you need to use the Xcode generator")
  endif ()
endif ()