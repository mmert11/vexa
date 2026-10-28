cmake_minimum_required(VERSION 3.20)
project(vexa VERSION 1.4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VEXA_BUILD_CLI "Build vexa-cli executable" ON)
option(VEXA_BUILD_TESTS "Build vexa test suite" OFF)

include(FetchContent)
include(GNUInstallDirs)

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

# CLI11 (header-only)
find_package(CLI11 QUIET CONFIG)
if(NOT CLI11_FOUND)
    find_path(CLI11_INCLUDE_DIR NAMES CLI/CLI.hpp PATH_SUFFIXES include share/include)
    if(CLI11_INCLUDE_DIR)
        add_library(CLI11::CLI11 INTERFACE IMPORTED GLOBAL)
        set_target_properties(CLI11::CLI11 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CLI11_INCLUDE_DIR}"
        )
    else()
        FetchContent_Declare(
            CLI11
            GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
            GIT_TAG        v2.4.2
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(CLI11)
    endif()
endif()

# Quill (async logging)
find_package(quill QUIET CONFIG)
if(NOT quill_FOUND)
    find_library(QUILL_LIB NAMES quill libquill PATH_SUFFIXES lib)
    find_path(QUILL_INC NAMES quill/Backend.h PATH_SUFFIXES include)
    if(QUILL_LIB AND QUILL_INC)
        add_library(quill::quill STATIC IMPORTED GLOBAL)
        set_target_properties(quill::quill PROPERTIES
            IMPORTED_LOCATION "${QUILL_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${QUILL_INC}"
        )
    else()
        FetchContent_Declare(
            quill
            GIT_REPOSITORY https://github.com/odygrd/quill.git
            GIT_TAG        v12.0.0
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(quill)
        if(TARGET quill AND NOT TARGET quill::quill)
            add_library(quill::quill ALIAS quill)
        endif()
    endif()
endif()

# LIEF executable format parser
find_package(LIEF QUIET CONFIG)
if(NOT LIEF_FOUND)
    find_library(LIEF_LIB NAMES LIEF libLIEF PATH_SUFFIXES lib)
    find_path(LIEF_INC NAMES LIEF/LIEF.hpp PATH_SUFFIXES include)
    find_library(SPDLOG_LIB NAMES spdlog libspdlog PATH_SUFFIXES lib)
    find_library(MBEDTLS_LIB NAMES mbedtls libmbedtls PATH_SUFFIXES lib)
    find_library(MBEDX509_LIB NAMES mbedx509 libmbedx509 PATH_SUFFIXES lib)
    find_library(MBEDCRYPTO_LIB NAMES mbedcrypto libmbedcrypto PATH_SUFFIXES lib)

    if(LIEF_LIB AND LIEF_INC)
        add_library(LIEF::LIEF STATIC IMPORTED GLOBAL)
        set(LIEF_DEPS_LIBS "")
        if(SPDLOG_LIB)
            list(APPEND LIEF_DEPS_LIBS "${SPDLOG_LIB}")
        endif()
        if(MBEDTLS_LIB)
            list(APPEND LIEF_DEPS_LIBS "${MBEDTLS_LIB}" "${MBEDX509_LIB}" "${MBEDCRYPTO_LIB}")
        endif()
        set_target_properties(LIEF::LIEF PROPERTIES
            IMPORTED_LOCATION "${LIEF_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIEF_INC}"
            INTERFACE_LINK_LIBRARIES "${LIEF_DEPS_LIBS}"
        )
        target_compile_definitions(LIEF::LIEF INTERFACE SPDLOG_COMPILED_LIB SPDLOG_NO_EXCEPTIONS)
    else()
        set(LIEF_DOC OFF CACHE BOOL "" FORCE)
        set(LIEF_PYTHON_API OFF CACHE BOOL "" FORCE)
        set(LIEF_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(LIEF_TESTS OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            LIEF
            GIT_REPOSITORY https://github.com/lief-project/LIEF.git
            GIT_TAG        0.17.6
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(LIEF)
        if(TARGET LIB_LIEF AND NOT TARGET LIEF::LIEF)
            add_library(LIEF::LIEF ALIAS LIB_LIEF)
        endif()
    endif()
endif()

# LIEF public header dependencies (tl-expected, tcb-span, utfcpp, frozen, spdlog)
find_path(TL_EXPECTED_INC NAMES tl/expected.hpp PATH_SUFFIXES include)
if(NOT TL_EXPECTED_INC)
    FetchContent_Declare(tl_expected GIT_REPOSITORY https://github.com/TartanLlama/expected.git GIT_TAG v1.1.0 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(tl_expected)
else()
    include_directories(SYSTEM ${TL_EXPECTED_INC})
endif()

find_path(TCB_SPAN_INC NAMES tcb/span.hpp PATH_SUFFIXES include)
if(NOT TCB_SPAN_INC)
    FetchContent_Declare(tcb_span GIT_REPOSITORY https://github.com/tcbrindle/span.git GIT_TAG master GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(tcb_span)
else()
    include_directories(SYSTEM ${TCB_SPAN_INC})
endif()

find_path(UTFCPP_INC NAMES utf8.h PATH_SUFFIXES include include/utf8cpp)
if(NOT UTFCPP_INC)
    FetchContent_Declare(utfcpp GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git GIT_TAG v4.0.5 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(utfcpp)
else()
    include_directories(SYSTEM ${UTFCPP_INC})
endif()

find_path(FROZEN_INC NAMES frozen/map.h PATH_SUFFIXES include)
if(FROZEN_INC)
    include_directories(SYSTEM ${FROZEN_INC})
endif()

find_path(SPDLOG_INC NAMES spdlog/spdlog.h PATH_SUFFIXES include)
if(SPDLOG_INC)
    include_directories(SYSTEM ${SPDLOG_INC})
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
