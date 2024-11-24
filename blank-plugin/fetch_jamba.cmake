cmake_minimum_required(VERSION 3.19)

include(FetchContent)

if(JAMBA_ROOT_DIR)
  # instructs FetchContent to not download or update but use the location instead
  set(FETCHCONTENT_SOURCE_DIR_JAMBA ${JAMBA_ROOT_DIR})
else()
  set(FETCHCONTENT_SOURCE_DIR_JAMBA "")
endif()

set(JAMBA_GIT_REPO "https://github.com/pongasoft/jamba" CACHE STRING "Jamba git repository url")
set(JAMBA_GIT_TAG [-jamba_git_hash-] CACHE STRING "Jamba git tag")
set(JAMBA_DOWNLOAD_URL "${JAMBA_GIT_REPO}/archive/refs/tags/[-jamba_git_hash-].zip" CACHE STRING "Jamba download url")
set(JAMBA_DOWNLOAD_URL_HASH "[-jamba_download_url_hash-]" CACHE STRING "Jamba download url hash")

if(JAMBA_DOWNLOAD_URL STREQUAL "" OR JAMBA_DOWNLOAD_URL_HASH STREQUAL "")
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
  set(JAMBA_FETCH_SOURCE "${JAMBA_GIT_REPO}/tree/${JAMBA_GIT_TAG}")
else()
  FetchContent_Declare(jamba
      URL                        "${JAMBA_DOWNLOAD_URL}"
      URL_HASH                   "${JAMBA_DOWNLOAD_URL_HASH}"
      DOWNLOAD_EXTRACT_TIMESTAMP true
      SOURCE_DIR                 "${CMAKE_BINARY_DIR}/jamba"
      BINARY_DIR                 "${CMAKE_BINARY_DIR}/jamba-build"
      CONFIGURE_COMMAND          ""
      BUILD_COMMAND              ""
      INSTALL_COMMAND            ""
      TEST_COMMAND               ""
      )
  set(JAMBA_FETCH_SOURCE "${JAMBA_DOWNLOAD_URL}")
endif()



FetchContent_GetProperties(jamba)

if(NOT jamba_POPULATED)

  if(FETCHCONTENT_SOURCE_DIR_JAMBA)
    message(STATUS "Using jamba from local ${FETCHCONTENT_SOURCE_DIR_JAMBA}")
  else()
    message(STATUS "Fetching jamba from ${JAMBA_FETCH_SOURCE}")
  endif()

  FetchContent_Populate(jamba)

endif()

set(JAMBA_ROOT_DIR ${jamba_SOURCE_DIR})

