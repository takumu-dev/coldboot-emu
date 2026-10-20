// SPDX-License-Identifier: GPL-3.0-or-later

/// \file
/// A `constexpr` encoder of R3000A instructions, so that tests build their guest code in C++.
///
/// There is one function per instruction, named by its lower-case mnemonic, taking its operands in
/// assembly order and returning its 32-bit word. There are no labels and no assembler. The
/// encodings are psx-spx's "CPU Opcode Encoding".

#pragma once

#include <coldboot/core/assertion.hpp>
#include <coldboot/core/cpu_state.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>

namespace coldboot::test {

/// A general-purpose register, by its conventional name. Its value is its number, r0 to r31.
enum class Gpr : std::uint8_t {
    zero, ///< r0, which always reads 0.
    at,   ///< r1.
    v0,   ///< r2.
    v1,   ///< r3.
    a0,   ///< r4.
    a1,   ///< r5.
    a2,   ///< r6.
    a3,   ///< r7.
    t0,   ///< r8.
    t1,   ///< r9.
    t2,   ///< r10.
    t3,   ///< r11.
    t4,   ///< r12.
    t5,   ///< r13.
    t6,   ///< r14.
    t7,   ///< r15.
    s0,   ///< r16.
    s1,   ///< r17.
    s2,   ///< r18.
    s3,   ///< r19.
    s4,   ///< r20.
    s5,   ///< r21.
    s6,   ///< r22.
    s7,   ///< r23.
    t8,   ///< r24.
    t9,   ///< r25.
    k0,   ///< r26.
    k1,   ///< r27.
    gp,   ///< r28.
    sp,   ///< r29.
    fp,   ///< r30.
    ra,   ///< r31.
};

/// The general-purpose registers by name, as `reg::t0`.
namespace reg {
using enum Gpr;
} // namespace reg

/// The value of a general-purpose register in a CPU state.
///
/// \param state The CPU state.
/// \param r The register.
/// \return Its value.
[[nodiscard]] constexpr std::uint32_t gpr(const CpuState& state, Gpr r) {
    return state.gpr[std::to_underlying(r)];
}

namespace detail {

/// An I-type instruction: an opcode, two registers and a 16-bit immediate, in the R3000 manual's
/// fields.
///
/// \param op The opcode, bits 26 to 31.
/// \param rs The register in bits 21 to 25.
/// \param rt The register in bits 16 to 20.
/// \param immediate The immediate, bits 0 to 15.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t i_type(std::uint32_t op, Gpr rs, Gpr rt,
                                             std::uint16_t immediate) {
    return op << 26 | std::uint32_t{std::to_underlying(rs)} << 21 |
           std::uint32_t{std::to_underlying(rt)} << 16 | immediate;
}

/// An R-type instruction with SPECIAL's opcode, 0, in the R3000 manual's fields.
///
/// \param rs The register in bits 21 to 25.
/// \param rt The register in bits 16 to 20.
/// \param rd The register in bits 11 to 15.
/// \param shamt The shift amount, bits 6 to 10, from 0 to 31.
/// \param funct The function, bits 0 to 5.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t special(Gpr rs, Gpr rt, Gpr rd, std::uint32_t shamt,
                                              std::uint32_t funct) {
    COLDBOOT_ASSERT(shamt < 32, "a shift amount is 0 to 31");
    return std::uint32_t{std::to_underlying(rs)} << 21 |
           std::uint32_t{std::to_underlying(rt)} << 16 |
           std::uint32_t{std::to_underlying(rd)} << 11 | shamt << 6 | funct;
}

} // namespace detail

/// `sll rd, rt, shamt`: shifts `rt` left by `shamt` bits into `rd`.
///
/// \param rd The destination.
/// \param rt The register shifted.
/// \param shamt The shift amount, from 0 to 31.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t sll(Gpr rd, Gpr rt, std::uint32_t shamt) {
    return detail::special(reg::zero, rt, rd, shamt, 0x00);
}

/// `nop`: `sll zero, zero, 0`, the all-zero word.
///
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t nop() { return sll(reg::zero, reg::zero, 0); }

/// `addiu rt, rs, immediate`: adds the sign-extended immediate to `rs` into `rt`, wrapping, with no
/// overflow trap.
///
/// \param rt The destination.
/// \param rs The register added to.
/// \param immediate The signed immediate.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t addiu(Gpr rt, Gpr rs, std::int16_t immediate) {
    return detail::i_type(0x09, rs, rt, static_cast<std::uint16_t>(immediate));
}

/// `ori rt, rs, immediate`: ORs the zero-extended immediate with `rs` into `rt`.
///
/// \param rt The destination.
/// \param rs The register ORed with.
/// \param immediate The unsigned immediate.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t ori(Gpr rt, Gpr rs, std::uint16_t immediate) {
    return detail::i_type(0x0D, rs, rt, immediate);
}

/// `lui rt, immediate`: puts the immediate in the upper half of `rt` and zeros in the lower.
///
/// \param rt The destination.
/// \param immediate The upper half.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t lui(Gpr rt, std::uint16_t immediate) {
    return detail::i_type(0x0F, reg::zero, rt, immediate);
}

/// `sw rt, offset(base)`: stores `rt` as a word at `base` plus the sign-extended offset.
///
/// \param rt The register stored.
/// \param offset The signed offset.
/// \param base The register holding the base address.
/// \return The instruction's word.
[[nodiscard]] constexpr std::uint32_t sw(Gpr rt, std::int16_t offset, Gpr base) {
    return detail::i_type(0x2B, base, rt, static_cast<std::uint16_t>(offset));
}

} // namespace coldboot::test
