# Copyright (c) 2021 pongasoft
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

cmake_minimum_required(VERSION 3.19)

include(FetchContent)

if(GOOGLETEST_ROOT_DIR)
  # instructs FetchContent to not download or update but use the location instead
  set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST ${GOOGLETEST_ROOT_DIR})
else()
  set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST "")
endif()

FetchContent_Declare(googletest
    GIT_REPOSITORY    ${googletest_GIT_REPO}
    GIT_TAG           ${googletest_GIT_TAG}
    GIT_CONFIG        advice.detachedHead=false
    SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
    BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
)

FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)

  if(FETCHCONTENT_SOURCE_DIR_GOOGLETEST)
    message(STATUS "Using googletest from local ${FETCHCONTENT_SOURCE_DIR_GOOGLETEST}")
  else()
    message(STATUS "Fetching googletest ${googletest_GIT_REPO}/tree/${googletest_GIT_TAG}")
  endif()

  FetchContent_Populate(googletest)

endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "Set by Jamba" FORCE)

# Do not install GoogleTest!
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
