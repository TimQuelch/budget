cmake_minimum_required(VERSION 3.20)

# Main project
project(budget
  LANGUAGES CXX
  VERSION 0.1.0
  )

# Compile options
include(${CMAKE_SOURCE_DIR}/cmake/set_build_options.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(frontend)
