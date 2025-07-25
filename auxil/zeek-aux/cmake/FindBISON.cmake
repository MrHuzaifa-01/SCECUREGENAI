# - Find bison executable and provides macros to generate custom build rules
# The module defines the following variables:
#
#  BISON_EXECUTABLE - path to the bison program
#  BISON_VERSION - version of bison
#  BISON_FOUND - true if the program was found
#
# If bison is found, the module defines the macros:
#  BISON_TARGET(<Name> <YaccInput> <CodeOutput> [VERBOSE <file>]
#              [COMPILE_FLAGS <string>] [HEADER <FILE>])
# which will create  a custom rule to generate  a parser. <YaccInput> is
# the path to  a yacc file. <CodeOutput> is the name  of the source file
# generated by bison.  A header file containing the token  list is  also
# generated according to bison's -d option by default  or if the  HEADER
# option is used, the argument is passed to  bison's --defines option to
# specify output file.  If COMPILE_FLAGS option is specified,  the  next
# parameter is  added in the bison  command line.  if  VERBOSE option is
# specified, <file> is created  and contains verbose descriptions of the
# grammar and parser. The macro defines a set of variables:
#  BISON_${Name}_DEFINED - true is the macro ran successfully
#  BISON_${Name}_INPUT - The input source file, an alias for <YaccInput>
#  BISON_${Name}_OUTPUT_SOURCE - The source file generated by bison
#  BISON_${Name}_OUTPUT_HEADER - The header file generated by bison
#  BISON_${Name}_OUTPUTS - The sources files generated by bison
#  BISON_${Name}_COMPILE_FLAGS - Options used in the bison command line
#
#  ====================================================================
#  Example:
#
#   find_package(BISON)
#   BISON_TARGET(MyParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
#   add_executable(Foo main.cpp ${BISON_MyParser_OUTPUTS})
#  ====================================================================

#=============================================================================
# Copyright 2009 Kitware, Inc.
# Copyright 2006 Tristan Carel
# Modified 2010 by Jon Siwek, adding HEADER option
#
# Distributed under the OSI-approved BSD License (the "License"):
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2009 Kitware, Inc., Insight Software Consortium
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

find_program(BISON_EXECUTABLE NAMES bison win-bison win_bison DOC "path to the bison executable")
MARK_AS_ADVANCED(BISON_EXECUTABLE)

IF(BISON_EXECUTABLE)

  EXECUTE_PROCESS(COMMAND ${BISON_EXECUTABLE} --version
    OUTPUT_VARIABLE BISON_version_output
    ERROR_VARIABLE BISON_version_error
    RESULT_VARIABLE BISON_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(NOT ${BISON_version_result} EQUAL 0)
    MESSAGE(SEND_ERROR "Command \"${BISON_EXECUTABLE} --version\" failed with output:\n${BISON_version_error}")
  ELSE()
    STRING(REGEX REPLACE "^bison \\(GNU [bB]ison\\) ([^\n]+)\n.*" "\\1"
      BISON_VERSION "${BISON_version_output}")
  ENDIF()

  # internal macro
  MACRO(BISON_TARGET_option_verbose Name BisonOutput filename)
    LIST(APPEND BISON_TARGET_cmdopt "--verbose")
    GET_FILENAME_COMPONENT(BISON_TARGET_output_path "${BisonOutput}" PATH)
    GET_FILENAME_COMPONENT(BISON_TARGET_output_name "${BisonOutput}" NAME_WE)
    ADD_CUSTOM_COMMAND(OUTPUT ${filename}
      COMMAND ${CMAKE_COMMAND}
      ARGS -E copy
      "${BISON_TARGET_output_path}/${BISON_TARGET_output_name}.output"
      "${filename}"
      DEPENDS
      "${BISON_TARGET_output_path}/${BISON_TARGET_output_name}.output"
      COMMENT "[BISON][${Name}] Copying bison verbose table to ${filename}"
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    SET(BISON_${Name}_VERBOSE_FILE ${filename})
    LIST(APPEND BISON_TARGET_extraoutputs
      "${BISON_TARGET_output_path}/${BISON_TARGET_output_name}.output")
  ENDMACRO(BISON_TARGET_option_verbose)

  # internal macro
  MACRO(BISON_TARGET_option_extraopts Options)
    SET(BISON_TARGET_extraopts "${Options}")
    SEPARATE_ARGUMENTS(BISON_TARGET_extraopts)
    LIST(APPEND BISON_TARGET_cmdopt ${BISON_TARGET_extraopts})
  ENDMACRO(BISON_TARGET_option_extraopts)

  #============================================================
  # BISON_TARGET (public macro)
  #============================================================
  #
  MACRO(BISON_TARGET Name BisonInput BisonOutput)
    SET(BISON_TARGET_output_header "")
    #SET(BISON_TARGET_command_opt "")
    SET(BISON_TARGET_cmdopt "")
    SET(BISON_TARGET_outputs "${BisonOutput}")
    IF(NOT ${ARGC} EQUAL 3 AND
       NOT ${ARGC} EQUAL 5 AND
       NOT ${ARGC} EQUAL 7 AND
       NOT ${ARGC} EQUAL 9)
      MESSAGE(SEND_ERROR "Usage")
    ELSE()
      # Parsing parameters
      IF(${ARGC} GREATER 5 OR ${ARGC} EQUAL 5)
        IF("${ARGV3}" STREQUAL "VERBOSE")
          BISON_TARGET_option_verbose(${Name} ${BisonOutput} "${ARGV4}")
        ENDIF()
        IF("${ARGV3}" STREQUAL "COMPILE_FLAGS")
          BISON_TARGET_option_extraopts("${ARGV4}")
        ENDIF()
        IF("${ARGV3}" STREQUAL "HEADER")
          set(BISON_TARGET_output_header "${ARGV4}")
        ENDIF()
      ENDIF()

      IF(${ARGC} GREATER 7 OR ${ARGC} EQUAL 7)
        IF("${ARGV5}" STREQUAL "VERBOSE")
          BISON_TARGET_option_verbose(${Name} ${BisonOutput} "${ARGV6}")
        ENDIF()

        IF("${ARGV5}" STREQUAL "COMPILE_FLAGS")
          BISON_TARGET_option_extraopts("${ARGV6}")
        ENDIF()

        IF("${ARGV5}" STREQUAL "HEADER")
          set(BISON_TARGET_output_header "${ARGV6}")
        ENDIF()
      ENDIF()

      IF(${ARGC} EQUAL 9)
        IF("${ARGV7}" STREQUAL "VERBOSE")
          BISON_TARGET_option_verbose(${Name} ${BisonOutput} "${ARGV8}")
        ENDIF()

        IF("${ARGV7}" STREQUAL "COMPILE_FLAGS")
          BISON_TARGET_option_extraopts("${ARGV8}")
        ENDIF()

        IF("${ARGV7}" STREQUAL "HEADER")
          set(BISON_TARGET_output_header "${ARGV8}")
        ENDIF()
      ENDIF()

      IF(BISON_TARGET_output_header)
          # Header's name passed in as argument to be used in --defines option
          LIST(APPEND BISON_TARGET_cmdopt
               "--defines=${BISON_TARGET_output_header}")
          set(BISON_${Name}_OUTPUT_HEADER ${BISON_TARGET_output_header})
      ELSE()
          # Header's name generated by bison (see option -d)
          LIST(APPEND BISON_TARGET_cmdopt "-d")
          STRING(REGEX REPLACE "^(.*)(\\.[^.]*)$" "\\2" _fileext "${ARGV2}")
          STRING(REPLACE "c" "h" _fileext ${_fileext})
          STRING(REGEX REPLACE "^(.*)(\\.[^.]*)$" "\\1${_fileext}"
              BISON_${Name}_OUTPUT_HEADER "${ARGV2}")
      ENDIF()

      LIST(APPEND BISON_TARGET_outputs "${BISON_${Name}_OUTPUT_HEADER}")

      ADD_CUSTOM_COMMAND(OUTPUT ${BISON_TARGET_outputs}
        ${BISON_TARGET_extraoutputs}
        COMMAND ${BISON_EXECUTABLE}
        ARGS ${BISON_TARGET_cmdopt} -o ${ARGV2} ${ARGV1}
        DEPENDS ${ARGV1}
        COMMENT "[BISON][${Name}] Building parser with bison ${BISON_VERSION}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

      # define target variables
      SET(BISON_${Name}_DEFINED TRUE)
      SET(BISON_${Name}_INPUT ${ARGV1})
      SET(BISON_${Name}_OUTPUTS ${BISON_TARGET_outputs})
      SET(BISON_${Name}_COMPILE_FLAGS ${BISON_TARGET_cmdopt})
      SET(BISON_${Name}_OUTPUT_SOURCE "${BisonOutput}")

    ENDIF(NOT ${ARGC} EQUAL 3 AND
          NOT ${ARGC} EQUAL 5 AND
          NOT ${ARGC} EQUAL 7 AND
          NOT ${ARGC} EQUAL 9)
  ENDMACRO(BISON_TARGET)
  #
  #============================================================

ENDIF(BISON_EXECUTABLE)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BISON DEFAULT_MSG BISON_EXECUTABLE)

# FindBISON.cmake ends here
