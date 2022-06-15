include(${CMAKE_SOURCE_DIR}/cmake/warnings.cmake)

find_program( CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable" )
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
endif()

function(budget_set_build_options target)
  target_compile_features(budget-tui PUBLIC cxx_std_20)
  target_compile_options(${target} PRIVATE ${warnings})

  if(CLANG_TIDY_EXE)
    # set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=*")
    set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
  endif()
endfunction()
