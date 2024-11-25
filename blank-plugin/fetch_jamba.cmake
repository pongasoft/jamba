cmake_minimum_required(VERSION 3.19)

include(FetchContent)

set(JAMBA_GIT_REPO "https://github.com/pongasoft/jamba" CACHE STRING "Jamba git repository url")
set(JAMBA_GIT_TAG [-jamba_git_hash-] CACHE STRING "Jamba git tag")
set(JAMBA_DOWNLOAD_URL "${JAMBA_GIT_REPO}/archive/refs/tags/[-jamba_git_hash-].zip" CACHE STRING "Jamba download url")
set(JAMBA_DOWNLOAD_URL_HASH "[-jamba_download_url_hash-]" CACHE STRING "Jamba download url hash")

if(JAMBA_ROOT_DIR)
  message(STATUS "Using jamba from local ${JAMBA_ROOT_DIR}")
  FetchContent_Populate(jamba
      QUIET
      SOURCE_DIR        "${JAMBA_ROOT_DIR}"
      BINARY_DIR        "${CMAKE_BINARY_DIR}/jamba-build"
  )
else()
  if(JAMBA_DOWNLOAD_URL STREQUAL "" OR JAMBA_DOWNLOAD_URL_HASH STREQUAL "")
    message(STATUS "Fetching jamba from ${JAMBA_GIT_REPO}/tree/${JAMBA_GIT_TAG}")
    FetchContent_Populate(jamba
        QUIET
        GIT_REPOSITORY    ${JAMBA_GIT_REPO}
        GIT_TAG           ${JAMBA_GIT_TAG}
        GIT_CONFIG        advice.detachedHead=false
        GIT_SHALLOW       true
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/jamba"
        BINARY_DIR        "${CMAKE_BINARY_DIR}/jamba-build"
    )
  else()
    message(STATUS "Fetching jamba from ${JAMBA_DOWNLOAD_URL}")
    FetchContent_Populate(jamba
        QUIET
        URL                        "${JAMBA_DOWNLOAD_URL}"
        URL_HASH                   "${JAMBA_DOWNLOAD_URL_HASH}"
        DOWNLOAD_EXTRACT_TIMESTAMP true
        SOURCE_DIR                 "${CMAKE_BINARY_DIR}/jamba"
        BINARY_DIR                 "${CMAKE_BINARY_DIR}/jamba-build"
    )
  endif()
endif()

set(JAMBA_ROOT_DIR ${jamba_SOURCE_DIR})

