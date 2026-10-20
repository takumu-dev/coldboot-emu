// SPDX-License-Identifier: GPL-3.0-or-later

#include "test_roms.hpp"

#include "r3000_encoder.hpp"
#include "rom_builder.hpp"

#include <memory>
#include <vector>

namespace coldboot::test {

std::unique_ptr<Rom> stop_rom() {
    return build_rom({
        lui(reg::t0, 0x1F80),
        ori(reg::t0, reg::t0, 0x2000),
        addiu(reg::t1, reg::zero, 0x41),
        sw(reg::t1, 0x20, reg::t0),
    });
}

std::vector<TestRom> test_roms() {
    std::vector<TestRom> roms;
    roms.push_back({.file_name = "stop.bin", .rom = stop_rom()});
    return roms;
}

} // namespace coldboot::test
