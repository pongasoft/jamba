cmake_minimum_required (VERSION 3.9) 

message(STATUS "Downloading jamba.. ${JAMBA_GIT_REPO}@${JAMBA_GIT_TAG}")
configure_file(${CMAKE_CURRENT_LIST_DIR}/jamba-download.cmake.in ${CMAKE_BINARY_DIR}/jamba-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/jamba-download )
if(result) 
  message(FATAL_ERROR "Error while setting up jamba download: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/jamba-download )
if(result)
  message(FATAL_ERROR "Error while downloading jamba: ${result}")
endif()

set(JAMBA_ROOT_DIR ${CMAKE_BINARY_DIR}/jamba)

