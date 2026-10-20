// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The C++-built ROMs that the unit tests run, and that the `test-roms` target writes to files for
/// the frontends.

#pragma once

#include "rom_builder.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace coldboot::test {

/// A ROM that builds the address of the absent DUART, 1F802020h, with LUI and ORI, a value with
/// ADDIU, and stores the value there with SW. So it stops at its fourth instruction, BFC0000Ch,
/// at emulated time 3.
///
/// \return The ROM.
[[nodiscard]] std::unique_ptr<Rom> stop_rom();

/// A ROM the `test-roms` target writes to a file.
struct TestRom {
    /// The file's name, without its directory.
    std::string_view file_name;

    /// The ROM.
    std::unique_ptr<Rom> rom;
};

/// Builds every ROM the `test-roms` target writes.
///
/// \return The ROMs, each with its file's name.
[[nodiscard]] std::vector<TestRom> test_roms();

} // namespace coldboot::test
