cmake_minimum_required(VERSION 3.19)

if (MAC)
  if (XCODE)
    set(AU_WRAPPER_ROOT "${vst3sdk_SOURCE_DIR}/public.sdk/source/vst/auwrapper")
    set(target jmb_auwrapper)
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
        )
    add_library(${target} STATIC ${auwrapper_sources})
    smtg_target_setup_universal_binary(${target})
    target_link_libraries(${target} PRIVATE
        sdk_hosting
        "-framework AudioUnit"
        "-framework CoreMIDI"
        "-framework AudioToolbox"
        "-framework CoreFoundation"
        "-framework Carbon"
        "-framework Cocoa"
        "-framework CoreAudio")
    target_include_directories(${target} PRIVATE "${SMTG_COREAUDIO_SDK_PATH}/**")
    string(MD5 AU_NAMESPACE_SUFFIX "${JAMBA_VST3_PLUGIN_TARGET}")
    file(WRITE "${CMAKE_BINARY_DIR}/generated/aucocoaclassprefix.h" "#define SMTG_AU_NAMESPACE SMTGAUCocoa${AU_NAMESPACE_SUFFIX}_\n")
    target_include_directories(${target} PRIVATE "${CMAKE_BINARY_DIR}/generated")
  else ()
    message("* To enable building the AudioUnit wrapper, you need to use the Xcode generator")
  endif ()
endif ()
