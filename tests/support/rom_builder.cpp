// SPDX-License-Identifier: GPL-3.0-or-later

#include "rom_builder.hpp"

#include <coldboot/core/assertion.hpp>
#include <coldboot/core/core_config.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <span>

namespace coldboot::test {

std::unique_ptr<Rom> build_rom(std::initializer_list<std::uint32_t> words) {
    COLDBOOT_ASSERT(words.size() <= bios_size / 4, "the words fit the ROM");
    auto rom = std::make_unique<Rom>();
    std::span<std::byte> free{*rom};
    for (const std::uint32_t word : words) {
        // The PS1 is little-endian, like every host the tests run on.
        std::ranges::copy(std::bit_cast<std::array<std::byte, 4>>(word), free.begin());
        free = free.subspan(4);
    }
    return rom;
}

} // namespace coldboot::test
