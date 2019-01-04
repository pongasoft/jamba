cmake_minimum_required(VERSION 3.12)

include(FetchContent)

if(JAMBA_ROOT_DIR)
  # instructs FetchContent to not download or update but use the location instead
  set(FETCHCONTENT_SOURCE_DIR_JAMBA ${JAMBA_ROOT_DIR})
else()
  set(FETCHCONTENT_SOURCE_DIR_JAMBA "")
endif()

set(JAMBA_GIT_REPO "https://github.com/pongasoft/jamba" CACHE STRING "Jamba git repository url" FORCE)
set(JAMBA_GIT_TAG [-jamba_git_hash-] CACHE STRING "Jamba git tag" FORCE)

FetchContent_Declare(jamba
      GIT_REPOSITORY    ${JAMBA_GIT_REPO}
      GIT_TAG           ${JAMBA_GIT_TAG}
      GIT_CONFIG        advice.detachedHead=false
      GIT_SHALLOW       true
      SOURCE_DIR        "${CMAKE_BINARY_DIR}/jamba"
      BINARY_DIR        "${CMAKE_BINARY_DIR}/jamba-build"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND     ""
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      )

FetchContent_GetProperties(jamba)

if(NOT jamba_POPULATED)

  if(FETCHCONTENT_SOURCE_DIR_JAMBA)
    message(STATUS "Using jamba from local ${FETCHCONTENT_SOURCE_DIR_JAMBA}")
  else()
    message(STATUS "Fetching jamba ${JAMBA_GIT_REPO}@${JAMBA_GIT_TAG}")
  endif()

  FetchContent_Populate(jamba)

endif()

set(JAMBA_ROOT_DIR ${jamba_SOURCE_DIR})

# finally we include jamba itself
include(${JAMBA_ROOT_DIR}/jamba.cmake)
