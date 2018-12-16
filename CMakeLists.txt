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
