cmake_minimum_required(VERSION 3.20)

# Dependencies

include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v3.0.0
  GIT_SHALLOW ON
  )
set(FTXUI_BUILD_DOCS OFF)
set(FTXUI_BUILD_EXAMPLES OFF)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.10.5
  GIT_SHALLOW ON
  )

FetchContent_Declare(cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr
  GIT_TAG 1.8.3
  GIT_SHALLOW ON
  )

FetchContent_MakeAvailable(ftxui json cpr)

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
target_compile_features(budget-tui PRIVATE cxx_std_20)
target_link_libraries(budget-tui
  PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component
  cpr::cpr
  nlohmann_json::nlohmann_json
  )
