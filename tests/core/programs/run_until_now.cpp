// SPDX-License-Identifier: GPL-3.0-or-later

#include "precondition_test.hpp"
#include "recording_diagnostics.hpp"
#include "rom_builder.hpp"
#include <coldboot/core/core.hpp>
#include <coldboot/core/core_config.hpp>

#include <memory>

void coldboot::test::break_precondition() {
    coldboot::test::RecordingDiagnostics diagnostics;
    const auto rom = coldboot::test::build_rom({});
    const auto core = std::make_unique<coldboot::Core>(
        coldboot::CoreConfig{.bios = *rom, .video_clock = {}, .diagnostics = diagnostics});

    static_cast<void>(core->run(core->now()));
}
