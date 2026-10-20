# SPDX-License-Identifier: GPL-3.0-or-later

# The compiler settings every target builds with. Everything here is PRIVATE to the target.
# Warnings become errors through CMAKE_COMPILE_WARNING_AS_ERROR, which every preset sets.

include_guard(GLOBAL)

# Turns on the warnings, an explicit list rather than -Weverything, so that a newer Clang builds the
# same tree. Third-party headers come in as SYSTEM includes, so these apply to our own code alone.
# Recorded paths, such as an assertion's, are made relative to the repository.
function(coldboot_target_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow-all
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wcast-align
        -Wcast-qual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wdocumentation
        -Wglobal-constructors
        -Wexit-time-destructors
        -Wextra-semi
        -Wcomma
        -Wundef
        -Wzero-as-null-pointer-constant
        -Wunreachable-code-aggressive
        -Wmissing-prototypes
        -Wcovered-switch-default
        -Wunsafe-buffer-usage
        "-fmacro-prefix-map=${PROJECT_SOURCE_DIR}/="
    )
endfunction()

# Applies the settings of the core and the frontends: the warnings, with exceptions and RTTI off.
# Test programs take only the warnings, since Catch2 stops a test by throwing.
function(coldboot_target_conventions target)
    coldboot_target_warnings(${target})
    target_compile_options(${target} PRIVATE -fno-exceptions -fno-rtti)
endfunction()
