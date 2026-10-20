// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The bus: the memory map the CPU reaches memory through.

#pragma once

#include <coldboot/core/core_config.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

/// The core's internals, which a frontend never uses.
namespace coldboot::detail {

/// The size of RAM, 2 MiB.
inline constexpr std::size_t ram_size = 2'097'152;

/// The address in the physical address space that a guest's address reaches.
///
/// KUSEG's first 512 MiB, KSEG0 and KSEG1 each mirror the physical address space. Any other
/// address, in KSEG2 or the rest of KUSEG, has no physical address: it is returned as it is, and
/// maps to nothing.
///
/// \param address The guest's address.
/// \return The physical address, or `address` itself when it has none.
[[nodiscard]] std::uint32_t physical_address(std::uint32_t address);

/// The memory map: the BIOS ROM and RAM, at the physical addresses psx-spx's "Memory Map" gives
/// them.
///
/// RAM starts at 0 and the ROM at 1FC00000h. Every other address maps to nothing, and an access to
/// it is absent.
class Bus {
  public:
    /// Creates the memory map at power-on, with RAM zeroed.
    ///
    /// \param bios The BIOS, which is copied into the ROM.
    explicit Bus(std::span<const std::byte, bios_size> bios);

    /// Reads a word.
    ///
    /// \param address The guest's address, a multiple of 4.
    /// \return The word, or nothing when the address maps to nothing.
    [[nodiscard]] std::optional<std::uint32_t> read_word(std::uint32_t address) const;

    /// Writes a word.
    ///
    /// \param address The guest's address, a multiple of 4.
    /// \param value The word.
    /// \return Whether the address maps to anything; when it doesn't, nothing changes.
    [[nodiscard]] bool write_word(std::uint32_t address, std::uint32_t value);

  private:
    /// RAM.
    std::array<std::byte, ram_size> ram_{};

    /// The BIOS ROM.
    std::array<std::byte, bios_size> rom_{};
};

} // namespace coldboot::detail
