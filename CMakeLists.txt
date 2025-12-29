cmake_minimum_required(VERSION 3.8)
project(VEXA)

if(MSVC)
    add_compile_options(/w /W0)
    set(CMAKE_SUPPRESS_REGENERATION true)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "")
    set(CMAKE_MSVC_WARNING_LEVEL 0)
    add_compile_options(/EHsc)
endif()

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter)
endif() 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
find_package(Z3 REQUIRED CONFIG)
find_package(ZYDIS REQUIRED CONFIG)

add_library(VEXA STATIC
    "src/vexa.cpp"
    "src/context.cpp"
    "src/engine/engine.cpp"
    "src/ir/builder.cpp"
    "src/ir/recompiler.cpp"
    "src/memory/memory.cpp"
    "src/arch/arch.cpp"
    "src/arch/x64/cpu.cpp"
    "src/arch/x64/semantics.cpp"
    "src/value/value.cpp"
    "src/symex/symex.cpp"
)

llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    irreader
    passes
    transformutils
    ipo
    X86CodeGen
    X86Desc
    X86Info
    MC
    Target
    Object
)

target_link_libraries(VEXA PUBLIC ${LLVM_LIBS})
target_include_directories(VEXA PUBLIC ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(VEXA PRIVATE ${Z3_CXX_INCLUDE_DIRS})
target_link_libraries(VEXA PRIVATE ${Z3_LIBRARIES})

target_compile_definitions(VEXA PUBLIC ZYDIS_STATIC_BUILD)
set_property(TARGET VEXA PROPERTY CXX_STANDARD 20)

target_include_directories(VEXA PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS VEXA
    EXPORT VEXATargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    include/vexa
    DESTINATION include/
)

install(EXPORT VEXATargets
    FILE VEXATargets.cmake
    NAMESPACE VEXA::
    DESTINATION lib/cmake/VEXA
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/VEXAConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/VEXAConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/VEXAConfig.cmake"
    INSTALL_DESTINATION lib/cmake/VEXA
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/VEXAConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VEXAConfigVersion.cmake"
    DESTINATION lib/cmake/VEXA
)
