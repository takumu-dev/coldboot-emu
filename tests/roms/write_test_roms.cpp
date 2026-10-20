// SPDX-License-Identifier: GPL-3.0-or-later

// Writes each C++-built test ROM to a file of its own in the working directory, for the frontends
// and the tools that boot a ROM from a file.

#include "test_roms.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio> // NOLINT(misc-include-cleaner): stderr
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iterator>
#include <print>
#include <string>

// NOLINTNEXTLINE(bugprone-exception-escape): an exception fails the build step, as it should
int main() {
    for (const auto& test_rom : coldboot::test::test_roms()) {
        const std::string file_name{test_rom.file_name};
        std::ofstream file{file_name, std::ios::binary};
        std::ranges::transform(*test_rom.rom, std::ostreambuf_iterator<char>{file},
                               [](std::byte byte) { return static_cast<char>(byte); });
        file.close();
        if (!file) {
            std::println(stderr, "Couldn't write {}.", file_name);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
