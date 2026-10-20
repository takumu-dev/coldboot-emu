# SPDX-License-Identifier: GPL-3.0-or-later

# The `test-roms` target writes each C++-built test ROM to test-roms/<name>.bin in the build tree,
# for the frontends and the tools that boot a ROM from a file.

add_executable(coldboot_write_test_roms write_test_roms.cpp)
target_link_libraries(coldboot_write_test_roms PRIVATE
    coldboot_test_support
    coldboot_aborting_assertion
)
coldboot_target_test_conventions(coldboot_write_test_roms)

set(directory "${PROJECT_BINARY_DIR}/test-roms")
file(MAKE_DIRECTORY "${directory}")
add_custom_command(
    OUTPUT "${directory}/written.stamp"
    COMMAND coldboot_write_test_roms
    COMMAND "${CMAKE_COMMAND}" -E touch written.stamp
    WORKING_DIRECTORY "${directory}"
    DEPENDS coldboot_write_test_roms
    COMMENT "Writing the test ROMs"
    VERBATIM
)
add_custom_target(test-roms ALL DEPENDS "${directory}/written.stamp")
