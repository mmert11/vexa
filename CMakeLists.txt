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

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

#set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "Installation directory" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
find_package(Z3 REQUIRED CONFIG)
find_package(ZYDIS REQUIRED CONFIG)

add_library(VEXA STATIC
"include/VEXA/VEXA.h"
"src/VEXA.cpp" 
"src/Memory/Memory.cpp"
"src/SymbolicState/SymbolicState.cpp"
"src/Engine/Engine.cpp"
"src/Value/Value.cpp" 
"src/CPU/CPU.cpp"
"src/Lifter/Lifter.cpp"
"src/Lifter/X64Semantics.cpp"
"src/Engine/X64Semantics.cpp"
"src/Solver/Solver.cpp"
"src/PathManager/PathManager.cpp")

target_compile_definitions(VEXA PUBLIC ZYDIS_STATIC_BUILD)

llvm_map_components_to_libnames(LLVM_LIBS
core
support
irreader
passes
transformutils
ipo
)

target_link_libraries(VEXA PUBLIC ${LLVM_LIBS})
target_include_directories(VEXA PUBLIC ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

target_include_directories(VEXA PRIVATE ${Z3_CXX_INCLUDE_DIRS})
target_link_libraries(VEXA PRIVATE ${Z3_LIBRARIES})

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
    include/VEXA
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
