cmake_minimum_required (VERSION 3.12)

project(jamba)

# In order to use the jamba framework you should include jamba.cmake directly or in other words do
#    include(<path_to_jamba>/jamba.cmake)
# instead of
#    add_subdirectory(<path_to_jamba>)
#
# The purpose of this file is so that it can be compiled separately to develop and test

include(jamba.cmake)

set(test_libs base sdk vstgui_support)

jamba_add_test(jamba_test "${JAMBA_TEST_SRC_FILES}" "${JAMBA_CPP_FOR_TEST_SOURCES}" "${test_libs}")

find_package(Doxygen)
if(DOXYGEN_FOUND)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs/${JAMBA_MAJOR_VERSION}.${JAMBA_MINOR_VERSION}.${JAMBA_PATCH_VERSION}")
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


set(BLANK_PLUGIN_ARCHIVE_NAME jamba-blank-plugin-${JAMBA_GIT_TAG})
set(BLANK_PLUGIN_ARCHIVE_PATH ${CMAKE_BINARY_DIR}/archive)

add_custom_command(OUTPUT ${BLANK_PLUGIN_ARCHIVE_PATH}.zip
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BLANK_PLUGIN_ARCHIVE_PATH}
    COMMAND ${CMAKE_COMMAND} -E tar cvf ${BLANK_PLUGIN_ARCHIVE_PATH}/${BLANK_PLUGIN_ARCHIVE_NAME}.zip --format=zip .
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/blank-plugin
    )

add_custom_target(blank-plugin-archive
    DEPENDS ${BLANK_PLUGIN_ARCHIVE_PATH}.zip
    )
