# SPDX-License-Identifier: GPL-3.0-or-later

# coldboot-emu: the core library, which holds the emulated PS1, and its tests.
#
# Every supported build is a preset in CMakePresets.json, so a developer and CI run the same
# `cmake --workflow --preset <preset>`. Dependencies come only through find_package; the build never
# downloads anything.
cmake_minimum_required(VERSION 4.4.2)

project(coldboot-emu
    VERSION 0.1.0
    DESCRIPTION "A PS1 emulator that is the only software on an x86-64 PC"
    HOMEPAGE_URL "https://github.com/takumu-dev/coldboot-emu"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(ColdbootCompilerFloor)
include(ColdbootConventions)

coldboot_check_compiler_floor("${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}")

# Standard C++23 as ordinary headers: no compiler extensions, and no scanning for C++ modules,
# which nothing here uses. Each target asks for C++23 itself.
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(COLDBOOT_BUILD_TESTS "Build the tests and the test ROMs." ON)
option(COLDBOOT_CLANG_TIDY "Run clang-tidy on every source as it is compiled." OFF)

if(COLDBOOT_CLANG_TIDY)
    coldboot_find_llvm_tool(COLDBOOT_CLANG_TIDY_PROGRAM clang-tidy)
    if(NOT COLDBOOT_CLANG_TIDY_PROGRAM)
        message(FATAL_ERROR "COLDBOOT_CLANG_TIDY needs clang-tidy ${COLDBOOT_CLANG_FLOOR}.")
    endif()
    set(CMAKE_CXX_CLANG_TIDY "${COLDBOOT_CLANG_TIDY_PROGRAM}")
endif()

add_subdirectory(src)

if(COLDBOOT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include(ColdbootLint)
