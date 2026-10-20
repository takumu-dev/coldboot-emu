# SPDX-License-Identifier: GPL-3.0-or-later

# The one compiler coldboot-emu builds with, Clang, and its floor. The floor rises at milestones and
# never falls, and configuring with anything older stops here, rather than somewhere deep in a
# standard header. clang-format and clang-tidy are the floor's major exactly, since what they
# accept and how they format change between majors.

include_guard(GLOBAL)

# The oldest Clang major supported, and the major of clang-format and clang-tidy.
set(COLDBOOT_CLANG_FLOOR 22)

# Stops the configuration unless the C++ compiler with this CMake compiler id and version is Clang
# at or above the floor.
function(coldboot_check_compiler_floor id version)
    if(NOT id STREQUAL "Clang")
        message(FATAL_ERROR "coldboot-emu builds with Clang alone; this is ${id} ${version}.")
    endif()
    if(version VERSION_LESS COLDBOOT_CLANG_FLOOR)
        message(FATAL_ERROR
            "coldboot-emu needs Clang ${COLDBOOT_CLANG_FLOOR} or newer; this is Clang ${version}."
        )
    endif()
endfunction()

# Finds the LLVM tool `name` at the floor's major, preferring `<name>-<floor>`, and stores its path
# in `out_var`. A tool of any other major isn't used: `out_var` is then left false, with a warning
# naming the major found.
function(coldboot_find_llvm_tool out_var name)
    find_program(${out_var} NAMES ${name}-${COLDBOOT_CLANG_FLOOR} ${name})
    if(NOT ${out_var})
        return()
    endif()
    execute_process(
        COMMAND "${${out_var}}" --version
        OUTPUT_VARIABLE version_text
        ERROR_QUIET
    )
    if(version_text MATCHES "version ([0-9]+)\\." AND CMAKE_MATCH_1 EQUAL COLDBOOT_CLANG_FLOOR)
        return()
    endif()
    message(WARNING
        "${${out_var}} isn't ${name} ${COLDBOOT_CLANG_FLOOR}, so it isn't used: ${version_text}"
    )
    set(${out_var} "${out_var}-NOTFOUND" CACHE FILEPATH "" FORCE)
endfunction()
