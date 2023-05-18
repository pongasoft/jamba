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
# This module adds the jamba.py (and jamba.sh / jamba.bat shortcuts) convenient
# command line commands
# Must define jamba_add_dev_script
#------------------------------------------------------------------------
function(jamba_add_dev_script)
  # Determine python executable
  if (ARG_PYTHON3_EXECUTABLE)
    set(Python3_EXECUTABLE ${ARG_PYTHON3_EXECUTABLE})
  else ()
    find_package(Python3 COMPONENTS Interpreter)

    if (${Python3_EXECUTABLE} STREQUAL "Python3_EXECUTABLE-NOTFOUND")
      set(Python3_EXECUTABLE "python")
      message(STATUS "python library not found => using \"${Python3_EXECUTABLE}\" as the executable (make sure it is in the PATH)")
    endif ()
  endif ()

  if (APPLE)
    set(JAMBA_SCRIPT_FILE "jamba.sh.in")
  elseif (WIN32)
    set(JAMBA_SCRIPT_FILE "jamba.bat.in")
  endif ()

  set(JAMBA_SCRIPT_EXE "${CMAKE_BINARY_DIR}/jamba.py")

  configure_file("${JAMBA_ROOT}/scripts/${JAMBA_SCRIPT_FILE}" "${JAMBA_SCRIPT_COMMAND}" @ONLY)

  macro(cmake_option_to_python_bool cmake_opt python_opt)
    if ("${cmake_opt}")
      set("python_${python_opt}" "True")
    else ()
      set("python_${python_opt}" "False")
    endif ()
  endmacro()

  cmake_option_to_python_bool(${GENERATOR_IS_MULTI_CONFIG} GENERATOR_IS_MULTI_CONFIG)
  cmake_option_to_python_bool(${JAMBA_ENABLE_AUDIO_UNIT} JAMBA_ENABLE_AUDIO_UNIT)

  configure_file(${JAMBA_ROOT}/scripts/jamba.py.in "${JAMBA_SCRIPT_EXE}" @ONLY)

  message(STATUS "Jamba main script available at ${JAMBA_SCRIPT_COMMAND}. Use -h option for help.")
endfunction()
