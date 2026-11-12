add_subdirectory(consteval)
add_subdirectory(unit)
add_subdirectory(compile_fail)

uring_add_test(smoke_test unit/smoke_test.cc)
uring_add_compile_fail(smoke_compile_fail compile_fail/smoke_compile_fail.cc)

# A source caret snippet must not satisfy the diagnostic matcher.
uring_add_compile_fail(caret_diagnostic_control compile_fail/smoke_compile_fail.cc EXPECT_DIAGNOSTIC_REJECTION)
target_compile_definitions(caret_diagnostic_control PRIVATE URING_CARET_FAILURE_CONTROL=1)

if(URING_ENABLE_SANITIZERS)
  uring_add_test(ubsan_failure_control unit/smoke_test.cc)
  target_compile_definitions(ubsan_failure_control PRIVATE URING_UBSAN_FAILURE_CONTROL=1)
  set_tests_properties(ubsan_failure_control PROPERTIES WILL_FAIL TRUE)
endif()

# This object must stay free of sanitizer instrumentation even in sanitizer presets.
uring_add_codegen_probes(native_smoke_probe unit/smoke_test.cc)

uring_find_native_nm(native_smoke_nm)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/native_smoke_probe_audit.cmake" [=[
if(NOT PROBE_OBJECTS)
  message(FATAL_ERROR "No object files provided for native_smoke_probe_audit")
endif()
foreach(obj IN LISTS PROBE_OBJECTS)
  if(NOT EXISTS "${obj}")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" --build "${BUILD_DIR}" --target native_smoke_probe
      RESULT_VARIABLE build_rc)
    if(NOT build_rc EQUAL 0 OR NOT EXISTS "${obj}")
      message(FATAL_ERROR "Missing native_smoke_probe object file: ${obj}")
    endif()
  endif()
  execute_process(
    COMMAND "${NM_TOOL}" "${obj}"
    RESULT_VARIABLE nm_rc
    OUTPUT_VARIABLE nm_out
    ERROR_VARIABLE nm_err)
  if(NOT nm_rc EQUAL 0)
    message(FATAL_ERROR "nm failed on ${obj}: ${nm_err}")
  endif()
  if(nm_out MATCHES "(__asan|__ubsan|__cxa_throw)")
    message(FATAL_ERROR "Forbidden sanitizer or exception symbol in ${obj}:\n${nm_out}")
  endif()
endforeach()
]=])
add_test(
  NAME native_smoke_probe_audit
  COMMAND "${CMAKE_COMMAND}"
    "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
    "-DNM_TOOL=${native_smoke_nm}"
    "-DPROBE_OBJECTS=$<TARGET_OBJECTS:native_smoke_probe>"
    -P "${CMAKE_CURRENT_BINARY_DIR}/native_smoke_probe_audit.cmake")

enable_language(C)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/mixed_consumer.c" "int main(void) { return 0; }\n")
add_executable(mixed_c_consumer "${CMAKE_CURRENT_BINARY_DIR}/mixed_consumer.c")
target_link_libraries(mixed_c_consumer PRIVATE uring::uring)
uarch_configure_runtime(mixed_c_consumer)
target_compile_options(mixed_c_consumer PRIVATE -Wall -Wextra -Werror)
target_link_options(mixed_c_consumer PRIVATE -Wl,-z,stack-size=8388608)
add_test(NAME mixed_c_consumer COMMAND mixed_c_consumer)
