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
else()
  message(STATUS "Doxygen is not installed. Install it if you want to generate the documentation.")
endif()
