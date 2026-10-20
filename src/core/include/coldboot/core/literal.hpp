// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// Text from a string literal, in the form the core holds text.
///
/// Text in the core is `std::span<const char>`, with no terminating null: `<string_view>` doesn't
/// build in the UEFI image.

#pragma once

#include <cstddef>
#include <span>

/// The ColdbootEmu project.
namespace coldboot {

/// The text of a string literal, without its terminating null.
///
/// \tparam Size The literal's size, its terminating null included.
/// \param text The string literal.
/// \return Its characters, which live as long as the program.
template <std::size_t Size>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays): a literal is an array
[[nodiscard]] consteval std::span<const char> literal(const char (&text)[Size]) {
    return std::span<const char, Size>{text}.template first<Size - 1>();
}

} // namespace coldboot
