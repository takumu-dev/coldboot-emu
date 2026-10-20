# SPDX-License-Identifier: GPL-3.0-or-later

# The core library: the core, its components, and the pure functions the frontends share with it.
# It compiles freestanding, so that a hosted header fails its build here as it would in the UEFI
# image. Its headers stay usable from hosted code, such as the tests.

add_library(coldboot_core STATIC)
add_library(coldboot::core ALIAS coldboot_core)
target_sources(coldboot_core
    PRIVATE
        bus.cpp
        core.cpp
        cpu.cpp
        stop_report.cpp
    PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
        include/coldboot/core/access.hpp
        include/coldboot/core/assertion.hpp
        include/coldboot/core/core.hpp
        include/coldboot/core/core_config.hpp
        include/coldboot/core/cpu_state.hpp
        include/coldboot/core/detail/bus.hpp
        include/coldboot/core/detail/cpu.hpp
        include/coldboot/core/detail/guest_memory.hpp
        include/coldboot/core/diagnostics.hpp
        include/coldboot/core/literal.hpp
        include/coldboot/core/run_result.hpp
        include/coldboot/core/stop_report.hpp
        include/coldboot/core/text_writer.hpp
)
target_compile_features(coldboot_core PUBLIC cxx_std_23)
target_compile_options(coldboot_core PRIVATE -ffreestanding)
coldboot_target_conventions(coldboot_core)
