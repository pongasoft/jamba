
# This is copied from the VST3 SDK to remove the -Wsuggest-override option which triggers a warning
# message for EVERY single file compiled thus making the output unreadable!
macro(setupPlatformToolset)
    # deprecated
    if(SMTG_RENAME_ASSERT)
        add_compile_options(-DSMTG_RENAME_ASSERT=1)
    endif()

    #------------
    if(SMTG_LINUX)
        option(SMTG_ADD_ADDRESS_SANITIZER_CONFIG "Add AddressSanitizer Config (Linux only)" OFF)
        if(SMTG_ADD_ADDRESS_SANITIZER_CONFIG)
            set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES};ASan")
            add_compile_options($<$<CONFIG:ASan>:-DDEVELOPMENT=1>)
            add_compile_options($<$<CONFIG:ASan>:-fsanitize=address>)
            add_compile_options($<$<CONFIG:ASan>:-DVSTGUI_LIVE_EDITING=1>)
            add_compile_options($<$<CONFIG:ASan>:-g>)
            add_compile_options($<$<CONFIG:ASan>:-O0>)
            set(ASAN_LIBRARY asan)
            link_libraries($<$<CONFIG:ASan>:${ASAN_LIBRARY}>)
        endif()
    endif()

    #------------
    if(UNIX)
        if(XCODE)
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
            set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
            add_compile_options(-Winconsistent-missing-override)    # Suggest override when missing 
        elseif(SMTG_MAC)
            set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
            set(CMAKE_CXX_STANDARD 17)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
            link_libraries(c++)
        else()
            set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
            set(CMAKE_CXX_STANDARD 17)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-multichar")
            if(ANDROID)
                set(CMAKE_ANDROID_STL_TYPE c++_static)
                link_libraries(dl)
             else()
                link_libraries(stdc++fs pthread dl)
            endif()
        endif()
    #------------
    elseif(SMTG_WIN)
        add_definitions(-D_UNICODE)
        add_compile_options(/fp:fast)                   # Floating Point Model
        add_compile_options($<$<CONFIG:Release>:/Oi>)   # Enable Intrinsic Functions (Yes)
        add_compile_options($<$<CONFIG:Release>:/Ot>)   # Favor Size Or Speed (Favor fast code)
        add_compile_options($<$<CONFIG:Release>:/GF>)   # Enable String Pooling
        add_compile_options($<$<CONFIG:Release>:/EHa>)  # Enable C++ Exceptions
        add_compile_options($<$<CONFIG:Release>:/Oy>)   # Omit Frame Pointers
        #add_compile_options($<$<CONFIG:Release>:/Ox>)  # Optimization (/O2: Maximise Speed /0x: Full Optimization)
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
        set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
    endif()
endmacro()
