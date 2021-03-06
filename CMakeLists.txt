#
# Copyright (C) 2018 Codership Oy <info@codership.com>
#

cmake_minimum_required (VERSION 2.8)

# Parse version from version header file and store it into
# WSREP_LIB_VERSION.
file (READ "include/wsrep/version.hpp" ver)
string(REGEX MATCH "WSREP_LIB_VERSION_MAJOR ([0-9]*)" _ ${ver})
set(ver_major ${CMAKE_MATCH_1})
file (READ "include/wsrep/version.hpp" ver)
string(REGEX MATCH "WSREP_LIB_VERSION_MINOR ([0-9]*)" _ ${ver})
set(ver_minor ${CMAKE_MATCH_1})
file (READ "include/wsrep/version.hpp" ver)
string(REGEX MATCH "WSREP_LIB_VERSION_PATCH ([0-9]*)" _ ${ver})
set(ver_patch ${CMAKE_MATCH_1})
set(WSREP_LIB_VERSION "${ver_major}.${ver_minor}.${ver_patch}")
message(STATUS "Wsrep-lib version: ${WSREP_LIB_VERSION}")

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
  project(wsrep-lib VERSION ${WSREP_LIB_VERSION})
else()
  project(wsrep-lib)
endif()

include(CheckIncludeFile)

# Options

# Compile unit tests
option(WSREP_LIB_WITH_UNIT_TESTS "Compile unit tests" OFF)
if (WSREP_LIB_WITH_UNIT_TESTS)
  # Run tests automatically by default if compiled
  option(WSREP_LIB_WITH_AUTO_TEST "Run unit tests automatically after build" OFF)
endif()

# Build a sample program
option(WSREP_LIB_WITH_DBSIM "Compile sample dbsim program" ON)

option(WSREP_LIB_WITH_ASAN "Enable address sanitizer" OFF)
option(WSREP_LIB_WITH_TSAN "Enable thread sanitizer" OFF)

option(WSREP_LIB_WITH_DOCUMENTATION "Generate documentation" OFF)
option(WSREP_LIB_WITH_COVERAGE "Compile with coverage instrumentation" OFF)

option(WSREP_LIB_STRICT_BUILD_FLAGS "Compile with strict build flags" OFF)
option(WSREP_LIB_MAINTAINER_MODE "Fail compilation on any warnings" OFF)

# Compiler options

# Set std to C++0x/C++11 if superproject has not set standard yet
if (NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 11)
  string(FIND "${CMAKE_CXX_FLAGS}" "-std=" HAVE_STD)
  if (HAVE_STD EQUAL -1)
    if (CMAKE_VERSION VERSION_LESS "3.1")
      if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
          CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.1)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
      else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
      endif()
    else()
      set(CMAKE_CXX_STANDARD 11)
    endif()
  endif()
endif()

# CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Woverloaded-virtual -g")

if (WSREP_LIB_STRICT_BUILD_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
endif()
if (WSREP_LIB_MAINTAINER_MODE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# Set up include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/wsrep-API/v26")

if (WSREP_LIB_WITH_UNIT_TESTS)
  find_package(Boost 1.54.0 REQUIRED
    unit_test_framework
    )
endif()

if (WSREP_LIB_WITH_DBSIM)
  find_package(Boost 1.54.0 REQUIRED
    program_options
    filesystem
    thread
    )
endif()

# Coverage
# To produce a coverage report, call cmake with -DWITH_COVERAGE=ON,
# run
#
#   make
#   make test
#   make ExperimentalCoverage
#   make coverage_report
#
# The coverage report output will be in directory root index.html
#
if (WSREP_LIB_WITH_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()
if (WSREP_LIB_WITH_ASAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()
if (WSREP_LIB_WITH_TSAN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

add_custom_target(coverage_report
  lcov --capture --directory src --directory test --directory dbsim --directory include --output lcov.info --no-external
  COMMAND genhtml --output-directory coverage_report lcov.info)


if (WSREP_LIB_WITH_DOCUMENTATION)
  find_package(Doxygen REQUIRED)
  add_custom_target(doc ALL
    COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
    COMMENT "Generating documentation with Doxygen"
    VERBATIM)
endif()

add_subdirectory(src)
add_subdirectory(wsrep-API)
if (WSREP_LIB_WITH_UNIT_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
if (WSREP_LIB_WITH_DBSIM)
  add_subdirectory(dbsim)
endif()
