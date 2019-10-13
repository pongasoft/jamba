cmake_minimum_required (VERSION 3.12)

project(jamba)

# In order to use the jamba framework you should include jamba.cmake directly or in other words do
#    include(<path_to_jamba>/jamba.cmake)
# instead of
#    add_subdirectory(<path_to_jamba>)
#
# The purpose of this file is so that it can be compiled separately to develop and test

set(CMAKE_CXX_STANDARD 17)

#------------------------------------------------------------------------
# adds the test plugin as a subdirectory (which will end up including jamba.cmake)
#------------------------------------------------------------------------
add_subdirectory(jamba-test-plugin)

#------------------------------------------------------------------------
# test for jamba framework
#------------------------------------------------------------------------
file(COPY test.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(test_jamba
    COMMAND ctest -C $<CONFIG>
    WORKING_DIRECTORY "jamba-test-plugin"
    DEPENDS test_vst3
    )

#------------------------------------------------------------------------
# Extract version
#------------------------------------------------------------------------
get_directory_property(JAMBA_MAJOR_VERSION DIRECTORY jamba-test-plugin DEFINITION JAMBA_MAJOR_VERSION)
get_directory_property(JAMBA_MINOR_VERSION DIRECTORY jamba-test-plugin DEFINITION JAMBA_MINOR_VERSION)
get_directory_property(JAMBA_PATCH_VERSION DIRECTORY jamba-test-plugin DEFINITION JAMBA_PATCH_VERSION)
get_directory_property(JAMBA_VERSION DIRECTORY jamba-test-plugin DEFINITION JAMBA_VERSION)

#------------------------------------------------------------------------
# Doxygen => generate html documentation
#------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/${JAMBA_MAJOR_VERSION}.${JAMBA_MINOR_VERSION}.${JAMBA_PATCH_VERSION}")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/jamba_doxygen.css ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen" VERBATIM
      )
  add_custom_command(TARGET doc POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E echo "Documentation generated and available @ ${CMAKE_CURRENT_BINARY_DIR}/docs/${JAMBA_MAJOR_VERSION}.${JAMBA_MINOR_VERSION}.${JAMBA_PATCH_VERSION}/html/index.html"
      )
else()
  message(STATUS "Doxygen is not installed. Install it if you want to generate the documentation.")
endif()


#------------------------------------------------------------------------
# blank-plugin-archive => generates the zip file containing the blank
# plugin only (used on the https://jamba.dev website)
#------------------------------------------------------------------------
set(BLANK_PLUGIN_ARCHIVE_NAME jamba-blank-plugin-v${JAMBA_VERSION})
set(BLANK_PLUGIN_ARCHIVE_PATH ${CMAKE_BINARY_DIR}/archive)

# Generate the file list for blank-plugin in order to exclude .DS_Store files
file(GLOB_RECURSE BLANK_PLUGIN_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} blank-plugin/*)
list(FILTER BLANK_PLUGIN_FILES EXCLUDE REGEX "\\.DS_Store$")
list(JOIN BLANK_PLUGIN_FILES "\n" BLANK_PLUGIN_FILES)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/blank-plugin-files.txt ${BLANK_PLUGIN_FILES})

# Build the blank-plugin zip archive
add_custom_command(OUTPUT ${BLANK_PLUGIN_ARCHIVE_PATH}.zip
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BLANK_PLUGIN_ARCHIVE_PATH}
    COMMAND ${CMAKE_COMMAND} -E tar cvf ${BLANK_PLUGIN_ARCHIVE_PATH}/${BLANK_PLUGIN_ARCHIVE_NAME}.zip --files-from=${CMAKE_CURRENT_BINARY_DIR}/blank-plugin-files.txt --format=zip
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_custom_target(blank-plugin-archive
    DEPENDS ${BLANK_PLUGIN_ARCHIVE_PATH}.zip
    )
