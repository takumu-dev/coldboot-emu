# SPDX-License-Identifier: GPL-3.0-or-later

# The tests. Each carries exactly one CTest label, naming what it needs: so far only `unit`, which
# needs the CPU alone and no files. Each test's name begins with the component it tests, matching
# the tag on its Catch2 test case, or with `build` for the build's own checks.

find_package(Catch2 3 CONFIG REQUIRED)
include(Catch)

# Applies the settings of a test program: the warnings, with exceptions and RTTI left on for
# Catch2, whose test cases register themselves through constructors of namespace-scope objects.
function(coldboot_target_test_conventions target)
    coldboot_target_warnings(${target})
    target_compile_options(${target} PRIVATE -Wno-global-constructors -Wno-exit-time-destructors)
endfunction()

# Adds a component's Catch2 test cases as one program, registering each case with CTest under the
# component's name. A failed check aborts the case, as it would the desktop frontend.
function(coldboot_add_tests component)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "SOURCES;LIBRARIES")
    set(target coldboot_${component}_tests)
    add_executable(${target} ${arg_SOURCES})
    target_link_libraries(${target} PRIVATE
        ${arg_LIBRARIES}
        coldboot_test_support
        coldboot_aborting_assertion
        Catch2::Catch2WithMain
    )
    coldboot_target_test_conventions(${target})
    catch_discover_tests(${target}
        TEST_PREFIX "${component}: "
        PROPERTIES LABELS unit
    )
endfunction()

# Adds a test that a precondition fires. A failed check ends the process, so each such test is a
# program of its own, sharing support/precondition_main.cpp, which CTest expects to fail. The source
# defines break_precondition() from support/precondition_test.hpp.
#
#   SOURCE     the program's one source file
#   LIBRARIES  what the program links besides the core
function(coldboot_add_precondition_test component name)
    cmake_parse_arguments(PARSE_ARGV 2 arg "" "SOURCE" "LIBRARIES")
    cmake_path(GET arg_SOURCE STEM stem)
    set(target coldboot_${component}_${stem})
    add_executable(${target} ${arg_SOURCE})
    target_link_libraries(${target} PRIVATE coldboot_precondition_main ${arg_LIBRARIES})
    coldboot_target_test_conventions(${target})
    set(test_name "${component}: ${name}")
    add_test(NAME "${test_name}" COMMAND ${target})
    set_tests_properties("${test_name}" PROPERTIES LABELS unit WILL_FAIL TRUE)
endfunction()

add_subdirectory(cmake)
add_subdirectory(support)
add_subdirectory(core)
add_subdirectory(roms)
