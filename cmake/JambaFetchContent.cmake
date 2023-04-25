# Copyright (c) 2023 pongasoft
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# @author Yan Pujante

cmake_minimum_required (VERSION 3.19)

include(FetchContent)

function(jamba_fetch_content)
  set(oneValueArgs NAME GIT_REPO GIT_TAG DOWNLOAD_URL ROOT_DIR)

  cmake_parse_arguments(
      "ARG" # prefix
      "" # options
      "${oneValueArgs}" # single values
      "" # multiple values
      ${ARGN}
  )

  if(NOT ARG_NAME)
    message(FATAL_ERROR "fetch_content requires NAME argument")
  endif()

  macro(set_default_value name default_value)
    if(NOT ${name})
      set(${name} "${default_value}")
    endif()
  endmacro()

  set_default_value(ARG_ROOT_DIR "${${ARG_NAME}_ROOT_DIR}")
  set_default_value(ARG_DOWNLOAD_URL "${${ARG_NAME}_DOWNLOAD_URL}")
  set_default_value(ARG_GIT_REPO "${${ARG_NAME}_GIT_REPO}")
  set_default_value(ARG_GIT_TAG  "${${ARG_NAME}_GIT_TAG}")
  set_default_value(ARG_GIT_TAG  "master")

  if(NOT ARG_ROOT_DIR AND NOT ARG_GIT_REPO AND NOT ARG_DOWNLOAD_URL)
    message(FATAL_ERROR "fetch_content requires either ROOT_DIR argument/${ARG_NAME}_ROOT_DIR variable or GIT_REPO/${ARG_NAME}_GIT_REPO or DOWNLOAD_URL/${ARG_NAME}_DOWNLOAD_URL to be defined ")
  endif()

  string(TOUPPER "${ARG_NAME}" UPPERCASE_NAME)

  set(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} ${ARG_ROOT_DIR})

  if(ARG_DOWNLOAD_URL)
    FetchContent_Declare(           ${ARG_NAME}
        URL                        "${ARG_DOWNLOAD_URL}"
        SOURCE_DIR                 "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}-src"
        BINARY_DIR                 "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}-build"
        DOWNLOAD_EXTRACT_TIMESTAMP true
        )
    set(FETCH_SOURCE "${ARG_DOWNLOAD_URL}")
  else()
    FetchContent_Declare(${ARG_NAME}
        GIT_REPOSITORY    ${ARG_GIT_REPO}
        GIT_TAG           ${ARG_GIT_TAG}
        GIT_CONFIG        advice.detachedHead=false
        GIT_SHALLOW       true
        SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}-src"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}-build"
        )
    set(FETCH_SOURCE "${ARG_GIT_REPO}/tree/${ARG_GIT_TAG}")
  endif()


  FetchContent_GetProperties(${ARG_NAME})

  if(NOT ${ARG_NAME}_POPULATED)
    if(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME})
      message(STATUS "Using ${ARG_NAME} from local ${FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME}}")
    else()
      message(STATUS "Fetching ${ARG_NAME} from ${FETCH_SOURCE}")
    endif()

    FetchContent_Populate(${ARG_NAME})
  endif()

  set(${ARG_NAME}_ROOT_DIR "${${ARG_NAME}_SOURCE_DIR}" PARENT_SCOPE)
  set(${ARG_NAME}_SOURCE_DIR "${${ARG_NAME}_SOURCE_DIR}" PARENT_SCOPE)
  set(${ARG_NAME}_BINARY_DIR "${${ARG_NAME}_BINARY_DIR}" PARENT_SCOPE)

endfunction()
