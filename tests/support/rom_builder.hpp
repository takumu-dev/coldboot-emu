// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// BIOS images built in C++, for tests that need no files and no MIPS toolchain.

#pragma once

#include <coldboot/core/core_config.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>

namespace coldboot::test {

/// A BIOS image, as big as the core takes.
using Rom = std::array<std::byte, bios_size>;

/// Builds a ROM that holds `words` from its start, at BFC00000h through KSEG1, and zero words after
/// them, each of which is a `nop`.
///
/// \param words The instructions or data, in order, no more than fill the ROM.
/// \return The ROM, on the heap, since it is too big for the stack.
[[nodiscard]] std::unique_ptr<Rom> build_rom(std::initializer_list<std::uint32_t> words);

} // namespace coldboot::test
