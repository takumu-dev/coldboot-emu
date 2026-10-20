// SPDX-License-Identifier: GPL-3.0-or-later

#include "rom_builder.hpp"

#include <coldboot/core/assertion.hpp>
#include <coldboot/core/core_config.hpp>
#include <coldboot/core/detail/guest_memory.hpp>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <span>

namespace coldboot::test {

std::unique_ptr<Rom> build_rom(std::initializer_list<std::uint32_t> words) {
    COLDBOOT_ASSERT(words.size() <= bios_size / 4, "the words fit the ROM");
    auto rom = std::make_unique<Rom>();
    std::size_t offset = 0;
    for (const std::uint32_t word : words) {
        detail::store(std::span{*rom}, offset, word);
        offset += 4;
    }
    return rom;
}

} // namespace coldboot::test
