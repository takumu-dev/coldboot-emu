// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// The core library's own writer of text into a fixed buffer.

#pragma once

#include <coldboot/core/assertion.hpp>
#include <coldboot/core/literal.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace coldboot {

/// Writes text into a buffer of its own, of a fixed capacity: literal text, and decimal and
/// hexadecimal numbers.
///
/// The core and the UEFI image have no heap, `<format>` or `<charconv>`, so they build their text
/// here. Writing more than the capacity holds breaks a precondition.
///
/// \tparam Capacity The most characters it holds.
template <std::size_t Capacity> class TextWriter {
  public:
    /// Writes `text` after what is already written.
    ///
    /// \param text The text, which must fit in what remains of the capacity.
    void write(std::span<const char> text) {
        COLDBOOT_ASSERT(text.size() <= Capacity - size_, "the text fits the writer's capacity");
        for (const char character : text) {
            buffer_[size_++] = character;
        }
    }

    /// Writes `value` in decimal, with no leading zeros.
    ///
    /// \param value The number, whose digits must fit in what remains of the capacity.
    void write_decimal(std::uint64_t value) {
        // 2⁶⁴ − 1 has 20 decimal digits. They are found from the last, and 0 has one.
        std::array<char, 20> digits{};
        std::size_t first = digits.size();
        while (first == digits.size() || value != 0) {
            digits[--first] = numerals[value % 10];
            value /= 10;
        }
        write(std::span{digits}.subspan(first));
    }

    /// Writes `value` in upper-case hexadecimal, as exactly `digits` digits with leading zeros.
    ///
    /// \param value The number, which must fit in `digits` hexadecimal digits.
    /// \param digits How many digits to write, from 1 to 8, which must fit in what remains of the
    ///               capacity.
    void write_hex(std::uint32_t value, std::size_t digits) {
        COLDBOOT_ASSERT(digits >= 1 && digits <= 8, "a hexadecimal number has 1 to 8 digits");
        COLDBOOT_ASSERT(digits == 8 || value >> (4 * digits) == 0,
                        "the number fits its hexadecimal digits");
        std::array<char, 8> text{};
        for (std::size_t i = 0; i < digits; ++i) {
            text[i] = numerals[(value >> (4 * (digits - 1 - i))) & 0xF];
        }
        write(std::span{text}.first(digits));
    }

    /// What has been written.
    ///
    /// \return The text, valid until the writer is next written to or destroyed.
    [[nodiscard]] std::span<const char> text() const { return std::span{buffer_}.first(size_); }

  private:
    /// The digits of every base written, by value.
    static constexpr std::array<char, 16> numerals{'0', '1', '2', '3', '4', '5', '6', '7',
                                                   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /// The characters written, in its first `size_` elements.
    std::array<char, Capacity> buffer_{};

    /// How many characters have been written.
    std::size_t size_ = 0;
};

} // namespace coldboot
