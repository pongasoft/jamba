# Copyright (c) 2020 pongasoft
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

#------------------------------------------------------------------------
# This module add testing (via Google Test)
# Must define jamba_add_test()
#------------------------------------------------------------------------
# Download and unpack googletest at configure time
include(JambaFetchGoogleTest)
include(GoogleTest)

#------------------------------------------------------------------------
# jamba_add_test - Testing
#------------------------------------------------------------------------
function(jamba_add_test)
  message(STATUS "Adding target ${ARG_TEST_TARGET} for test cases: ${ARG_TEST_CASE_SOURCES}")

  # required to run the tests (provide moduleHandle)
  if(APPLE)
    set(MAIN_TEST_SOURCES "${VST3_SDK_ROOT}/public.sdk/source/main/macmain.cpp")
  elseif(WIN32)
    set(MAIN_TEST_SOURCES "${VST3_SDK_ROOT}/public.sdk/source/main/dllmain.cpp")
  endif()

  add_executable("${ARG_TEST_TARGET}" "${ARG_TEST_CASE_SOURCES}" "${ARG_TEST_SOURCES}" "${MAIN_TEST_SOURCES}")
  target_link_libraries("${ARG_TEST_TARGET}" gtest_main "${ARG_TEST_LINK_LIBRARIES}")
  target_include_directories("${ARG_TEST_TARGET}" PUBLIC "${PROJECT_SOURCE_DIR}" "${ARG_TEST_INCLUDE_DIRECTORIES}")
  smtg_target_setup_universal_binary("${ARG_TEST_TARGET}")

  # Extra compile definitions?
  if(ARG_TEST_COMPILE_DEFINITIONS)
    target_compile_definitions("${ARG_TEST_TARGET}" PUBLIC "${ARG_TEST_COMPILE_DEFINITIONS}")
  endif()

  # Extra compile options?
  if(ARG_TEST_COMPILE_OPTIONS)
    target_compile_options("${ARG_TEST_TARGET}" PUBLIC "${ARG_TEST_COMPILE_OPTIONS}")
  endif()

  gtest_discover_tests("${ARG_TEST_TARGET}")

  #------------------------------------------------------------------------
  # test_vst3 target | can be changed by setting TEST_VST3_TARGET before calling this function
  #------------------------------------------------------------------------
  if (NOT TEST_VST3_TARGET)
    add_custom_target("${ARG_TARGETS_PREFIX}test_vst3"
        COMMAND ${CMAKE_COMMAND} -E echo "Running tests using $<TARGET_FILE:${ARG_TEST_TARGET}>"
        COMMAND "${CMAKE_CTEST_COMMAND}" -C $<CONFIG>
        DEPENDS "${ARG_TEST_TARGET}"
        )
  else()
    add_custom_target("${ARG_TARGETS_PREFIX}test_vst3" DEPENDS "${TEST_VST3_TARGET}"
        )
  endif ()
endfunction()

