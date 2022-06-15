cmake_minimum_required(VERSION 3.20)

# Dependencies

include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v3.0.0
  GIT_SHALLOW ON
  )

set(FTXUI_BUILD_DOCS OFF CACHE BOOL "Don't build docs")
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "Don't build examples")
set(FTXUI_CLANG_TIDY OFF CACHE BOOL "Don't clang-tidy dependency")

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.10.5
  GIT_SHALLOW ON
  )

set(JSON_ImplicitConversions OFF CACHE BOOL "Disable implicit conversions")
set(JSON_BuildTests OFF CACHE BOOL "Don't build tests")
set(JSON_SystemInclude ON CACHE BOOL "Treat headers as system to remove noisy diagnostics")

FetchContent_Declare(cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr
  GIT_TAG 1.8.3
  GIT_SHALLOW ON
  )

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 8.1.1
  GIT_SHALLOW ON
  )

FetchContent_MakeAvailable(ftxui json cpr fmt)

# TUI executable
add_executable(budget-tui tui/main.cpp)
budget_set_build_options(budget-tui)
target_include_directories(budget-tui PRIVATE tui)
target_link_libraries(budget-tui
  PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component
  cpr::cpr
  nlohmann_json::nlohmann_json
  fmt::fmt
  )
