#------------------------------------------------------------------------
# Testing
#------------------------------------------------------------------------
# Download and unpack googletest at configure time
include(JambaFetchGoogleTest)
enable_testing()
include(GoogleTest)

#------------------------------------------------------------------------
# jamba_add_test - Testing
#------------------------------------------------------------------------
function(jamba_add_test PROJECT_TEST_NAME TEST_CASES_FILES TEST_SOURCES TEST_LIBS)
  message(STATUS "Adding target ${PROJECT_TEST_NAME} for test cases: ${TEST_CASES_FILES}")

  if (WIN)
    set(WIN_SOURCES "${JAMBA_ROOT}/windows/testmain.cpp")
  endif ()

  add_executable(${PROJECT_TEST_NAME} ${TEST_CASES_FILES} ${TEST_SOURCES} ${WIN_SOURCES})
  target_link_libraries(${PROJECT_TEST_NAME} gtest_main ${TEST_LIBS})
  target_include_directories(${PROJECT_TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
  target_include_directories(${PROJECT_TEST_NAME} PUBLIC ${GTEST_INCLUDE_DIRS})

  gtest_add_tests(
      TARGET ${PROJECT_TEST_NAME}
      TEST_LIST ${PROJECT_TEST_NAME}_targets
  )
endfunction()
