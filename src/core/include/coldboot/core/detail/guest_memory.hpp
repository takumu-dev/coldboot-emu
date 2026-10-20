// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The only way into guest memory: loads and stores of little-endian values in bytes.
///
/// RAM and the BIOS ROM are arrays of bytes, and every value in them is read and written here,
/// through `std::bit_cast`. Nothing casts a pointer into guest memory.

#pragma once

#include <coldboot/core/assertion.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <span>

// The PS1 is little-endian, and guest memory holds its values as the host's own.
static_assert(std::endian::native == std::endian::little, "the host is little-endian");

namespace coldboot::detail {

/// Loads a value from guest memory.
///
/// \tparam T The unsigned type of the value, as wide as the access.
/// \param memory The guest memory.
/// \param offset Where the value's lowest byte is, with the whole value inside `memory`.
/// \return The value.
template <std::unsigned_integral T>
[[nodiscard]] T load(std::span<const std::byte> memory, std::size_t offset) {
    COLDBOOT_DEBUG_ASSERT(offset <= memory.size() && memory.size() - offset >= sizeof(T),
                          "the value lies inside the memory");
    std::array<std::byte, sizeof(T)> bytes{};
    std::ranges::copy(memory.subspan(offset, sizeof(T)), bytes.begin());
    return std::bit_cast<T>(bytes);
}

/// Stores a value in guest memory.
///
/// \tparam T The unsigned type of the value, as wide as the access.
/// \param memory The guest memory.
/// \param offset Where the value's lowest byte goes, with the whole value inside `memory`.
/// \param value The value.
template <std::unsigned_integral T>
void store(std::span<std::byte> memory, std::size_t offset, T value) {
    COLDBOOT_DEBUG_ASSERT(offset <= memory.size() && memory.size() - offset >= sizeof(T),
                          "the value lies inside the memory");
    std::ranges::copy(std::bit_cast<std::array<std::byte, sizeof(T)>>(value),
                      memory.subspan(offset, sizeof(T)).begin());
}

} // namespace coldboot::detail
