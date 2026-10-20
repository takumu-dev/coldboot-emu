# SPDX-License-Identifier: GPL-3.0-or-later

# The build's own checks, tested by running them as scripts.

# Adds a test that runs the compiler floor check on one compiler. A compiler it refuses must stop
# the configuration with a message saying why (FAILS_WITH); one at the floor must pass.
function(coldboot_add_floor_test name id version)
    cmake_parse_arguments(PARSE_ARGV 3 arg "" "FAILS_WITH" "")
    set(test_name "build: ${name}")
    add_test(NAME "${test_name}"
        COMMAND "${CMAKE_COMMAND}"
            "-DCOMPILER_ID=${id}"
            "-DCOMPILER_VERSION=${version}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compiler_floor.cmake"
    )
    set_tests_properties("${test_name}" PROPERTIES LABELS unit)
    if(DEFINED arg_FAILS_WITH)
        # The message is the behaviour under test: CTest then passes on it alone, whatever the
        # exit status, and only FATAL_ERROR prints it.
        set_tests_properties("${test_name}" PROPERTIES PASS_REGULAR_EXPRESSION "${arg_FAILS_WITH}")
    endif()
endfunction()

coldboot_add_floor_test("configuring with Clang 21 fails, naming the floor" Clang 21.1.8
    FAILS_WITH "coldboot-emu needs Clang 22 or newer; this is Clang 21\\.1\\.8\\."
)
coldboot_add_floor_test("configuring with Clang 22 passes" Clang 22.1.0)
coldboot_add_floor_test("configuring with GCC fails, since Clang alone builds coldboot-emu"
    GNU 16.2.1
    FAILS_WITH "coldboot-emu builds with Clang alone; this is GNU 16\\.2\\.1\\."
)

# Adds a test that runs the SPDX check on a fresh repository, as check_spdx_fixture.cmake makes it,
# with the badly headed file BAD if it is given. The check must pass, or fail with output matching
# FAILS_WITH.
function(coldboot_add_spdx_test name)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "BAD;FAILS_WITH" "")
    string(MAKE_C_IDENTIFIER "${name}" id)
    set(options
        "-DGIT=${GIT_EXECUTABLE}"
        "-DDIRECTORY=${CMAKE_CURRENT_BINARY_DIR}/spdx/${id}"
    )
    if(DEFINED arg_BAD)
        list(APPEND options "-DBAD=${arg_BAD}")
    endif()
    set(test_name "build: ${name}")
    add_test(NAME "${test_name}"
        COMMAND "${CMAKE_COMMAND}" ${options}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_spdx_fixture.cmake"
    )
    set_tests_properties("${test_name}" PROPERTIES LABELS unit)
    if(DEFINED arg_FAILS_WITH)
        set_tests_properties("${test_name}" PROPERTIES PASS_REGULAR_EXPRESSION "${arg_FAILS_WITH}")
    endif()
    if(NOT Git_FOUND)
        set_tests_properties("${test_name}" PROPERTIES DISABLED TRUE)
    endif()
endfunction()

find_package(Git)
coldboot_add_spdx_test("the SPDX check passes when each source and build file begins with its line")
coldboot_add_spdx_test("the SPDX check fails on a C++ header without its line first, naming it"
    BAD include/thing.hpp
    FAILS_WITH "These files don't begin with their SPDX line:[ \n]+include/thing\\.hpp"
)
coldboot_add_spdx_test("the SPDX check fails on a CMake file without its line first, naming it"
    BAD cmake/Thing.cmake
    FAILS_WITH "These files don't begin with their SPDX line:[ \n]+cmake/Thing\\.cmake"
)
