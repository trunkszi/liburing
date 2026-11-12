include_guard(GLOBAL)

# Native codegen evidence is compiled separately from instrumented tests.
function(uring_add_codegen_probes target)
  add_library(${target} OBJECT ${ARGN})
  uring_configure_internal(${target} NO_SANITIZERS)
  set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
  target_compile_options(${target} PRIVATE -O3 -fno-lto)
  # Global coverage settings must not change native instruction evidence.
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${target} PRIVATE -fno-profile-instr-generate -fno-coverage-mapping)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${target} PRIVATE -fno-profile-arcs -fno-test-coverage -fno-profile-generate)
  endif()
endfunction()

function(uring_find_native_nm output)
  find_program(native_nm NAMES llvm-nm
    HINTS "${URING_TOOLCHAIN_ROOT}/clang-glibc/bin" "$ENV{HOME}/.local/toolchain/clang-glibc/bin")
  if(NOT native_nm AND CMAKE_NM)
    set(native_nm "${CMAKE_NM}")
  endif()
  set(${output} "${native_nm}" PARENT_SCOPE)
endfunction()
