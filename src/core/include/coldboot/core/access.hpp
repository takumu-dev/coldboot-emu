// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The direction and width of a guest's access to memory or a device.

#pragma once

#include <cstdint>

namespace coldboot {

/// Whether an access reads or writes.
enum class AccessDirection : std::uint8_t {
    read,  ///< A load, or the fetch of an instruction.
    write, ///< A store.
};

/// How many bits an access moves at once.
enum class AccessWidth : std::uint8_t {
    byte,     ///< 8 bits.
    halfword, ///< 16 bits.
    word,     ///< 32 bits.
};

} // namespace coldboot
