// SPDX-License-Identifier: GPL-3.0-or-later

#include <coldboot/core/detail/bus.hpp>

#include <coldboot/core/assertion.hpp>
#include <coldboot/core/core_config.hpp>
#include <coldboot/core/detail/guest_memory.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace coldboot::detail {

namespace {

// psx-spx "Memory Map": where the BIOS ROM starts in the physical address space.
constexpr std::uint32_t rom_start = 0x1FC0'0000;

} // namespace

std::uint32_t physical_address(std::uint32_t address) {
    // psx-spx "Memory Map": KUSEG's first 512 MiB (segment 0), KSEG0 (segment 4, from 80000000h)
    // and KSEG1 (segment 5, from A0000000h) mirror the physical address space. Any other address
    // stays as it is, which puts it beyond RAM and the ROM.
    const std::uint32_t segment = address >> 29;
    if (segment == 0 || segment == 4 || segment == 5) {
        return address & 0x1FFF'FFFF;
    }
    return address;
}

Bus::Bus(std::span<const std::byte, bios_size> bios) { std::ranges::copy(bios, rom_.begin()); }

std::optional<std::uint32_t> Bus::read_word(std::uint32_t address) const {
    COLDBOOT_DEBUG_ASSERT(address % 4 == 0, "a word's address is a multiple of 4");
    const std::uint32_t physical = physical_address(address);
    if (physical < ram_size) {
        return load<std::uint32_t>(ram_, physical);
    }
    if (physical - rom_start < bios_size) {
        return load<std::uint32_t>(rom_, physical - rom_start);
    }
    return std::nullopt;
}

bool Bus::write_word(std::uint32_t address, std::uint32_t value) {
    COLDBOOT_DEBUG_ASSERT(address % 4 == 0, "a word's address is a multiple of 4");
    const std::uint32_t physical = physical_address(address);
    if (physical < ram_size) {
        store(ram_, physical, value);
        return true;
    }
    // Guess: a write to the ROM is dropped, since the ROM can't hold it.
    return physical - rom_start < bios_size;
}

} // namespace coldboot::detail
