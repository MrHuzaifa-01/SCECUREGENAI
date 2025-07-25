cmake_minimum_required(VERSION 3.5)

set(HU_STANDALONE_PROJECT FALSE)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(HU_STANDALONE_PROJECT TRUE)
endif()

if(HU_STANDALONE_PROJECT)
  project(header-utils LANGUAGES C CXX)
endif()

add_library(header-utils INTERFACE)
target_include_directories(header-utils
                           INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")
add_library(header-utils::header-utils ALIAS header-utils)

if(HU_STANDALONE_PROJECT)
  macro(hu_add_test tgt)
    add_executable(${tgt} test/${ARGN})
    target_link_libraries(${tgt} header-utils::header-utils)
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
      target_compile_options(${tgt} PRIVATE -Wundef)
    endif()
  endmacro()

  set(tests
      bits
      compiler
      endian
      lang
      arch
      macros
      os
      selftest)
  foreach(x ${tests})
    hu_add_test(${x} "${x}.cpp")
  endforeach()
endif()
