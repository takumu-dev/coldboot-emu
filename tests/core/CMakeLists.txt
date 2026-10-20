# SPDX-License-Identifier: GPL-3.0-or-later

coldboot_add_tests(core
    SOURCES
        core_tests.cpp
        stop_report_tests.cpp
        text_writer_tests.cpp
    LIBRARIES
        coldboot::core
)

coldboot_add_precondition_test(core "a run to a time no later than now fires its precondition"
    SOURCE programs/run_until_now.cpp
    LIBRARIES coldboot::core
)

# A source that includes a hosted header, compiled with the core library's own options. Building it
# is the test, which passes only on libstdc++'s refusal, so it is never built otherwise and is kept
# out of the compile database.
add_library(coldboot_core_hosted_header OBJECT EXCLUDE_FROM_ALL hosted_header.cpp)
target_link_libraries(coldboot_core_hosted_header PRIVATE coldboot::core)
target_compile_options(coldboot_core_hosted_header PRIVATE
    $<TARGET_PROPERTY:coldboot_core,COMPILE_OPTIONS>
)
set_target_properties(coldboot_core_hosted_header PROPERTIES EXPORT_COMPILE_COMMANDS OFF)
set(test_name "core: including a hosted header fails the core's build")
add_test(NAME "${test_name}"
    COMMAND "${CMAKE_COMMAND}" --build "${PROJECT_BINARY_DIR}" --target coldboot_core_hosted_header
)
set_tests_properties("${test_name}" PROPERTIES
    LABELS unit
    PASS_REGULAR_EXPRESSION "This header is not available in freestanding mode"
)
