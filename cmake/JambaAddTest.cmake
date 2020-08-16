#------------------------------------------------------------------------
# Testing
#------------------------------------------------------------------------
# Download and unpack googletest at configure time
include(JambaFetchGoogleTest)
include(GoogleTest)

#------------------------------------------------------------------------
# internal_jamba_add_test - Testing
#------------------------------------------------------------------------
function(internal_jamba_add_test)
  message(STATUS "Adding target ${ARG_TEST_TARGET} for test cases: ${ARG_TEST_CASE_SOURCES}")

  if (WIN)
    set(WIN_SOURCES "${JAMBA_ROOT}/windows/testmain.cpp")
  endif ()

  add_executable("${ARG_TEST_TARGET}" "${ARG_TEST_CASE_SOURCES}" "${ARG_TEST_SOURCES}" "${WIN_SOURCES}")
  target_link_libraries("${ARG_TEST_TARGET}" gtest_main "${ARG_TEST_LINK_LIBRARIES}")
  target_include_directories("${ARG_TEST_TARGET}" PUBLIC "${PROJECT_SOURCE_DIR}" "${GTEST_INCLUDE_DIRS}" "${ARG_TEST_INCLUDE_DIRECTORIES}")

  # Extra compile definitions?
  if(ARG_TEST_COMPILE_DEFINITIONS)
    target_compile_definitions("${ARG_TEST_TARGET}" PUBLIC "${ARG_TEST_COMPILE_DEFINITIONS}")
  endif()

  # Extra compile options?
  if(ARG_TEST_COMPILE_OPTIONS)
    target_compile_options("${ARG_TEST_TARGET}" PUBLIC "${ARG_TEST_COMPILE_OPTIONS}")
  endif()

  gtest_add_tests(
      TARGET "${ARG_TEST_TARGET}"
      TEST_LIST "${ARG_TEST_TARGET}_targets"
  )

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

# invoke function
internal_jamba_add_test()