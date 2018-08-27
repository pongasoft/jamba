cmake_minimum_required (VERSION 3.9)

# build VST 2 by default
option(SMTG_CREATE_VST2_VERSION "Use VST2" ON)
option(SMTG_CREATE_VST3_LINK "" OFF)

set(JAMBA_GIT_REPO "https://github.com/pongasoft/jamba" CACHE STRING "Jamba git repository url" FORCE)
set(JAMBA_GIT_TAG [-jamba_git_hash-] CACHE STRING "Jamba git tag" FORCE)

set(PLUGIN_MAJOR_VERSION 1)
set(PLUGIN_MINOR_VERSION 0)
set(PLUGIN_PATCH_VERSION 0)
set(PLUGIN_VERSION "${PLUGIN_MAJOR_VERSION}.${PLUGIN_MINOR_VERSION}.${PLUGIN_PATCH_VERSION}")

project([-name-])

# for working on the framework at the same time...
#set(JAMBA_ROOT_DIR ../jamba)

if(NOT JAMBA_ROOT_DIR)
	include(jamba.cmake)
endif()

include(${JAMBA_ROOT_DIR}/jamba.cmake)

set(CPP_SOURCES src/cpp)

configure_file(${CPP_SOURCES}/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

if (SMTG_CREATE_VST2_VERSION)
	set(vst2_sources
			${CPP_SOURCES}/[-name-]_VST2.cpp
			)
endif()

set(vst_sources
    ${CPP_SOURCES}/[-name-]_VST3.cpp

    ${CPP_SOURCES}/[-name-].h
    ${CPP_SOURCES}/[-name-]CIDs.h

		${CPP_SOURCES}/RT/[-name-]Processor.h
		${CPP_SOURCES}/RT/[-name-]Processor.cpp

		${CPP_SOURCES}/GUI/[-name-]Controller.h
		${CPP_SOURCES}/GUI/[-name-]Controller.cpp
  )

# VST2 is only defined for macOS and Windows
if(MAC OR WIN)
	set(vst_sources ${vst_sources} ${vst2_sources})
endif()

set(target [-company-]_[-name-])
smtg_add_vst3plugin(${target} ${VST3_SDK_ROOT} ${vst_sources})
target_include_directories(${target} PUBLIC ${VSTGUI_ROOT}/vstgui4)
target_link_libraries(${target} PRIVATE base sdk vstgui_support jamba)

# smtg_add_vst3_resource(${target} "resource/background.png")
smtg_add_vst3_resource(${target} "resource/[-name-].uidesc")

if(MAC)
	smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/mac/Info.plist" PREPROCESS)
elseif(WIN)
	target_sources(${target} PRIVATE resource/[-name-].rc)
endif()

# fix for vst2
jamba_fix_vst2(${target})

###################################################
# Create archive (.tgz)
###################################################
jamba_create_archive(${target} [-filename-])

####################################################
## Testing
####################################################
file(GLOB_RECURSE TEST_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} test/cpp/*cpp)
set(test_sources ${test_sources}
		${LOGURU_IMPL}
#		${CPP_SOURCES}/Model.cpp
  )
set(test_libs base sdk vstgui_support jamba)

jamba_add_test(${target}_test "${TEST_SRC_FILES}" "${test_sources}" "${test_libs}")

###################################################
# Dev help
###################################################
jamba_dev_scripts(${target})
