cmake_minimum_required(VERSION 3.12)

# build VST 2?
option(SMTG_CREATE_VST2_VERSION "Use VST2" [-enable_vst2-])

# build Audio Unit?
option(JAMBA_ENABLE_AUDIO_UNIT "Enable Audio Unit" ON)

option(SMTG_CREATE_VST3_LINK "" OFF)

set(PLUGIN_MAJOR_VERSION 1)
set(PLUGIN_MINOR_VERSION 0)
set(PLUGIN_PATCH_VERSION 0)
set(PLUGIN_VERSION "${PLUGIN_MAJOR_VERSION}.${PLUGIN_MINOR_VERSION}.${PLUGIN_PATCH_VERSION}")

project([-name-])

# To use local jamba install, uncomment the following line (no download)
#set(JAMBA_ROOT_DIR "[-jamba_root_dir-]")

# To use local googletest install, uncomment the following line (no download) and modify the path accordingly
#set(GOOGLETEST_ROOT_DIR ../../google/googletest)

include(jamba.cmake)

set(CPP_SOURCES src/cpp)

configure_file(${CPP_SOURCES}/version.h.in ${CMAKE_BINARY_DIR}/generated/version.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

if (SMTG_CREATE_VST2_VERSION)
  set(vst2_sources
      ${CPP_SOURCES}/[-name-]_VST2.cpp
      )
endif ()

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
if (MAC OR WIN)
  set(vst_sources ${vst_sources} ${vst2_sources})
endif ()

set(target [-company-]_[-name-])
jamba_add_vst3plugin(${target} "${vst_sources}")
target_include_directories(${target} PUBLIC ${VSTGUI_ROOT}/vstgui4)
target_link_libraries(${target} PRIVATE base sdk vstgui_support jamba)

###################################################
# Resources - add your resources here...
###################################################
jamba_add_vst3_resource(${target} DATA "[-name-].uidesc")
# jamba_add_vst3_resource(${target} PNG "background.png")

if (MAC)
  smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/mac/Info.plist" PREPROCESS)
elseif (WIN)
  jamba_gen_vst3_resource_rc("${CMAKE_BINARY_DIR}/generated/vst3_resources.rc")
  target_sources(${target} PRIVATE resource/[-name-].rc)
endif ()

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
    #${CPP_SOURCES}/Model.cpp
    )
set(test_libs base sdk vstgui_support jamba)

jamba_add_test(${target}_test "${TEST_SRC_FILES}" "${test_sources}" "${test_libs}")

###################################################
# Dev help
###################################################
jamba_dev_scripts(${target})
