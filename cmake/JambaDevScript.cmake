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

  if (MAC)
    set(JAMBA_SCRIPT_FILE "jamba.sh")
  elseif (WIN)
    set(JAMBA_SCRIPT_FILE "jamba.bat")
  endif ()

  configure_file("${JAMBA_ROOT}/scripts/${JAMBA_SCRIPT_FILE}.in" "${CMAKE_BINARY_DIR}/${JAMBA_SCRIPT_FILE}" @ONLY)

  macro(cmake_option_to_python_bool cmake_opt python_opt)
    if ("${cmake_opt}")
      set("python_${python_opt}" "True")
    else ()
      set("python_${python_opt}" "False")
    endif ()
  endmacro()

  cmake_option_to_python_bool(${GENERATOR_IS_MULTI_CONFIG} GENERATOR_IS_MULTI_CONFIG)
  cmake_option_to_python_bool(${JAMBA_ENABLE_VST2} JAMBA_ENABLE_VST2)
  cmake_option_to_python_bool(${JAMBA_ENABLE_AUDIO_UNIT} JAMBA_ENABLE_AUDIO_UNIT)

  configure_file(${JAMBA_ROOT}/scripts/jamba.py.in ${CMAKE_BINARY_DIR}/jamba.py @ONLY)

  message(STATUS "Jamba main script available at ${CMAKE_BINARY_DIR}/${JAMBA_SCRIPT_FILE}. Run ${JAMBA_SCRIPT_FILE} -h for help.")
endfunction()
