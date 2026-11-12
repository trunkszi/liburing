cmake_minimum_required(VERSION 4.0)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/UringToolchain.cmake")
uring_ensure_toolchain()

project(uring VERSION 0.1.0 DESCRIPTION "C++26 runtime-free io_uring and futex2 proactor framework" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
option(URING_BUILD_TESTS "Build deterministic tests" ${PROJECT_IS_TOP_LEVEL})
option(URING_BUILD_EXAMPLES "Build examples" ${URING_BUILD_TESTS})
option(URING_BUILD_BENCHES "Build benchmarks" OFF)
option(URING_BUILD_TOOLS "Build codegen probes" ${URING_BUILD_TESTS})
option(URING_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(URING_INSTALL_PACKAGE "Install the uring CMake package" ON)
include(UringCompilerOptions)
include(UringReflection)
include(UringSanitizers)
include(UringTargets)
include(UringCompileFail)
include(UringCodegenGates)
uring_detect_reflection()
uring_create_libraries()

add_subdirectory(src)
if(URING_BUILD_TESTS)
  enable_testing()
endif()
if(URING_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
if(URING_BUILD_TESTS)
  add_subdirectory(tests)
endif()
if(URING_BUILD_BENCHES)
  add_subdirectory(benches)
endif()

if(URING_INSTALL_PACKAGE)
  install(TARGETS uring_error uring_abi uring_sys uring_caps uring_core uring_ring
    uring_ops uring_resources uring_futex2 uring_io uring EXPORT uring-targets)
  install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
  install(EXPORT uring-targets NAMESPACE uring:: DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/uring")
  configure_package_config_file(cmake/uring-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/uring-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/uring")
  write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/uring-config-version.cmake"
    VERSION "${PROJECT_VERSION}" COMPATIBILITY SameMajorVersion)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/uring-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/uring-config-version.cmake"
    cmake/UringToolchain.cmake cmake/UringReflection.cmake cmake/UringCompilerOptions.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/uring")
endif()
