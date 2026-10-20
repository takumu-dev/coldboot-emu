# SPDX-License-Identifier: GPL-3.0-or-later

# What the tests share, and the two ways a test program handles a failed check.

add_library(coldboot_test_support STATIC
    assertion_report.cpp
    rom_builder.cpp
    test_roms.cpp
)
target_include_directories(coldboot_test_support PUBLIC .)
target_link_libraries(coldboot_test_support PUBLIC coldboot::core Catch2::Catch2)
coldboot_target_test_conventions(coldboot_test_support)

# Reports a failed check and aborts: for Catch2 test programs and the test tools.
add_library(coldboot_aborting_assertion OBJECT aborting_assertion.cpp)
target_link_libraries(coldboot_aborting_assertion PUBLIC coldboot_test_support)
coldboot_target_test_conventions(coldboot_aborting_assertion)

# The main program every precondition test shares, which reports a failed check and exits failing.
add_library(coldboot_precondition_main OBJECT precondition_main.cpp)
target_link_libraries(coldboot_precondition_main PUBLIC coldboot_test_support)
coldboot_target_test_conventions(coldboot_precondition_main)

coldboot_add_tests(support
    SOURCES
        r3000_encoder_tests.cpp
)
