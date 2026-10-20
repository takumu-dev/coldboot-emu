// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The core's text as a string, for tests to compare.

#pragma once

#include <span>
#include <string>

namespace coldboot::test {

/// Copies text, as the core holds it, into a string.
///
/// \param text The text.
/// \return The same characters.
[[nodiscard]] inline std::string as_string(std::span<const char> text) {
    return {text.begin(), text.end()};
}

} // namespace coldboot::test
