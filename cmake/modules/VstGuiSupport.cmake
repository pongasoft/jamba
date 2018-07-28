## YP Note - This file is copied from the SDK to modifyt add_subdirectory which does not contain ${VSTGUI_ROOT} and fails
macro(setupVstGuiSupport)
    set(VSTGUI_DISABLE_UNITTESTS 1)
    set(VSTGUI_DISABLE_STANDALONE 1)
    set(VSTGUI_DISABLE_STANDALONE_EXAMPLES 1)
    add_subdirectory(${VSTGUI_ROOT}/vstgui4/vstgui vstgui4/vstgui)

    set(VST3_VSTGUI_SOURCES
        ${VSTGUI_ROOT}/vstgui4/vstgui/plugin-bindings/vst3groupcontroller.cpp
        ${VSTGUI_ROOT}/vstgui4/vstgui/plugin-bindings/vst3groupcontroller.h
        ${VSTGUI_ROOT}/vstgui4/vstgui/plugin-bindings/vst3padcontroller.cpp
        ${VSTGUI_ROOT}/vstgui4/vstgui/plugin-bindings/vst3padcontroller.h
        ${VSTGUI_ROOT}/vstgui4/vstgui/plugin-bindings/vst3editor.cpp
        ${VSTGUI_ROOT}/vstgui4/vstgui/plugin-bindings/vst3editor.h
        ${SDK_ROOT}/public.sdk/source/vst/vstguieditor.cpp
        )
    add_library(vstgui_support STATIC ${VST3_VSTGUI_SOURCES})
    target_compile_definitions(vstgui_support PUBLIC $<$<CONFIG:Debug>:VSTGUI_LIVE_EDITING=1>)
    target_include_directories(vstgui_support PUBLIC ${VSTGUI_ROOT}/vstgui4)
    target_link_libraries(vstgui_support PRIVATE vstgui_uidescription)
    smtg_setup_universal_binary(vstgui_support)
    smtg_setup_universal_binary(vstgui)
    smtg_setup_universal_binary(vstgui_uidescription)
    if(MAC)
        if(XCODE)
            target_link_libraries(vstgui_support PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework Accelerate" "-framework QuartzCore" "-framework Carbon")
        else()
            find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
            find_library(COCOA_FRAMEWORK Cocoa)
            find_library(OPENGL_FRAMEWORK OpenGL)
            find_library(ACCELERATE_FRAMEWORK Accelerate)
            find_library(QUARTZCORE_FRAMEWORK QuartzCore)
            find_library(CARBON_FRAMEWORK Carbon)
            target_link_libraries(vstgui_support PRIVATE ${COREFOUNDATION_FRAMEWORK} ${COCOA_FRAMEWORK} ${OPENGL_FRAMEWORK} ${ACCELERATE_FRAMEWORK} ${QUARTZCORE_FRAMEWORK} ${CARBON_FRAMEWORK})
        endif()
    endif()
endmacro()
