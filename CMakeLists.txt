cmake_minimum_required(VERSION 3.20)
project(vexa VERSION 1.4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VEXA_BUILD_CLI "Build vexa-cli executable" ON)
option(VEXA_BUILD_TESTS "Build vexa test suite" OFF)

include(GNUInstallDirs)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EXPECTED_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TCB_SPAN_BUILD_TESTS OFF CACHE BOOL "" FORCE)


# Compiler flags
if(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    add_compile_options(/O2 /W3 /DNOMINMAX)
else()
    add_compile_options(-O3 -Wall -fvisibility=hidden)
endif()

# External dependencies search paths
file(GLOB VEXA_EXTERNAL_DEPS_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/deps/*")
list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/external/deps"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/remill/install"
    ${VEXA_EXTERNAL_DEPS_DIRS}
)

# LLVM (user-supplied)
find_package(LLVM REQUIRED CONFIG)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFS_LIST})

set(LLVM_REQUIRED_COMPONENTS
    X86
    Core
    Support
    Passes
    Analysis
    TransformUtils
    InstCombine
    ScalarOpts
    Target
    MC
    MCParser
    AsmParser
    BitReader
    BitWriter
    IRReader
    Demangle
    GlobalISel
    SelectionDAG
    CodeGen
    Vectorize
    Instrumentation
    IPO
    Coroutines
)
llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_REQUIRED_COMPONENTS})

# CLI11 (header-only submodule)
set(CLI11_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/cli11)

# Quill (logging submodule)
set(QUILL_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/quill)
if(TARGET quill AND NOT TARGET quill::quill)
    add_library(quill::quill ALIAS quill)
endif()

# LIEF (executable parser submodule)
set(LIEF_DOC OFF CACHE BOOL "" FORCE)
set(LIEF_PYTHON_API OFF CACHE BOOL "" FORCE)
set(LIEF_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LIEF_TESTS OFF CACHE BOOL "" FORCE)
set(LIEF_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(external/lief)
if(TARGET LIB_LIEF AND NOT TARGET LIEF::LIEF)
    add_library(LIEF::LIEF ALIAS LIB_LIEF)
endif()

# Bitwright Rust library
set(BITWRIGHT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/bitwright")
if(NOT EXISTS "${BITWRIGHT_DIR}/Cargo.toml")
    find_package(Git REQUIRED)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --depth 1 https://github.com/valeria-org/bitwright.git "${BITWRIGHT_DIR}"
        RESULT_VARIABLE GIT_CLONE_RES
    )
    if(NOT GIT_CLONE_RES EQUAL 0)
        message(FATAL_ERROR "Failed to clone Bitwright from GitHub.")
    endif()
endif()

if(WIN32)
    set(BITWRIGHT_LIB "${BITWRIGHT_DIR}/target/release/bitwright.lib")
else()
    set(BITWRIGHT_LIB "${BITWRIGHT_DIR}/target/release/libbitwright.a")
endif()

add_custom_command(
    OUTPUT "${BITWRIGHT_LIB}"
    COMMAND cargo build --release -p bitwright-ffi
    WORKING_DIRECTORY "${BITWRIGHT_DIR}"
    COMMENT "Building Bitwright via Cargo..."
)
add_custom_target(bitwright_target DEPENDS "${BITWRIGHT_LIB}")

add_library(bitwright STATIC IMPORTED GLOBAL)
set_target_properties(bitwright PROPERTIES
    IMPORTED_LOCATION "${BITWRIGHT_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${BITWRIGHT_DIR}/bitwright-ffi/include"
)
add_dependencies(bitwright bitwright_target)

# Remill lifter and architecture runtime
set(REMILL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/remill")
if(NOT EXISTS "${REMILL_DIR}/CMakeLists.txt")
    find_package(Git REQUIRED)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --depth 1 https://github.com/lifting-bits/remill.git "${REMILL_DIR}"
        RESULT_VARIABLE GIT_REMILL_CLONE_RES
    )
    if(NOT GIT_REMILL_CLONE_RES EQUAL 0)
        message(FATAL_ERROR "Failed to clone Remill from GitHub.")
    endif()
endif()

set(REMILL_LIB_NAMES
    remill_arch
    remill_arch_x86
    remill_arch_aarch64
    remill_arch_sparc32
    remill_arch_sparc64
    remill_arch_sleigh
    remill_bc
    remill_os
    remill_version
    decomp
    sla
    slaSupport
    xed
    xed-ild
    glog
    gflags
)

find_path(REMILL_INCLUDE_DIR NAMES remill/Arch/Arch.h
    PATHS
        "${REMILL_INSTALL_DIR}/include"
        "${REMILL_DIR}/include"
        "${REMILL_DIR}/install/include"
)
find_path(REMILL_DEPS_INCLUDE_DIR NAMES gflags/gflags.h
    PATHS
        "${REMILL_INSTALL_DIR}/include"
        "${REMILL_DIR}/dependencies/install/include"
        "${REMILL_DIR}/install/include"
)

set(REMILL_LIBRARIES "")
foreach(lib_name ${REMILL_LIB_NAMES})
    find_library(LIB_${lib_name} NAMES ${lib_name} lib${lib_name}
        PATHS
            "${REMILL_INSTALL_DIR}/lib"
            "${REMILL_DIR}/lib"
            "${REMILL_DIR}/dependencies/install/lib"
            "${REMILL_DIR}/install/lib"
    )
    if(LIB_${lib_name})
        list(APPEND REMILL_LIBRARIES "${LIB_${lib_name}}")
    endif()
endforeach()

# System libraries
if(WIN32)
    set(VEXA_SYSTEM_LIBS ws2_32 dbghelp psapi user32 shell32 ole32 oleaut32 advapi32)
else()
    set(VEXA_SYSTEM_LIBS pthread dl m z zstd tinfo xml2)
endif()

# Target: vexa (static library)
file(GLOB_RECURSE VEXA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE VEXA_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(vexa STATIC ${VEXA_SOURCES} ${VEXA_HEADERS})

target_include_directories(vexa PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${BITWRIGHT_DIR}/bitwright-ffi/include>
    $<INSTALL_INTERFACE:include>
)

if(REMILL_INCLUDE_DIR)
    target_include_directories(vexa SYSTEM PUBLIC ${REMILL_INCLUDE_DIR})
endif()
if(REMILL_DEPS_INCLUDE_DIR)
    target_include_directories(vexa SYSTEM PUBLIC ${REMILL_DEPS_INCLUDE_DIR})
endif()

target_link_libraries(vexa PUBLIC
    bitwright
    LIEF::LIEF
    quill::quill
    ${REMILL_LIBRARIES}
    ${LLVM_LIBS}
    ${VEXA_SYSTEM_LIBS}
)

# Target: vexa-cli
if(VEXA_BUILD_CLI)
    add_executable(vexa-cli "${CMAKE_CURRENT_SOURCE_DIR}/cli/cli.cpp")
    target_link_libraries(vexa-cli PRIVATE vexa CLI11::CLI11)
endif()

# Target: tests pipeline
if(VEXA_BUILD_TESTS)
    enable_testing()

    file(GLOB_RECURSE TEST_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*/test.cpp")
    foreach(test_source ${TEST_CPP_FILES})
        get_filename_component(test_dir ${test_source} DIRECTORY)
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}/tests" ${test_dir})
        string(REPLACE "/" "-" test_name ${rel_path})
        set(target_name "test-${test_name}")

        add_executable(${target_name} ${test_source})
        target_link_libraries(${target_name} PRIVATE vexa)
        target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")

        add_test(NAME ${target_name} COMMAND ${target_name} WORKING_DIRECTORY ${test_dir})
    endforeach()
endif()
