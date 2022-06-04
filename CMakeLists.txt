cmake_minimum_required(VERSION 3.20)

# Dependencies

include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v3.0.0
  )

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Compile options
include(${CMAKE_SOURCE_DIR}/cmake/warnings.cmake)
add_compile_options(${warnings})
set(CMAKE_CXX_CLANG_TIDY clang-tidy)

# Main project

project(budget
  LANGUAGES CXX
  VERSION 0.1.0
  )

add_executable(budget-tui tui/main.cpp)
target_include_directories(budget-tui PRIVATE tui)
target_link_libraries(budget-tui PRIVATE ftxui::screen ftxui::dom ftxui::component)
target_compile_features(budget-tui PRIVATE cxx_std_20)
