// SPDX-License-Identifier: GPL-3.0-or-later

// The expected words are LLVM's own encodings (llvm-mc -triple=mipsel -mcpu=mips1), so that the
// encoder and the core's decoding can't agree on a mistake.

#include "r3000_encoder.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace coldboot::test;

TEST_CASE("the encoder encodes sll and nop as an assembler does", "[support]") {
    STATIC_CHECK(sll(reg::t2, reg::t1, 4) == 0x0009'5100);
    STATIC_CHECK(sll(reg::zero, reg::zero, 31) == 0x0000'07C0);
    STATIC_CHECK(nop() == 0x0000'0000);
}

TEST_CASE("the encoder encodes addiu, sign-extending its immediate, as an assembler does",
          "[support]") {
    STATIC_CHECK(addiu(reg::t1, reg::zero, 0x41) == 0x2409'0041);
    STATIC_CHECK(addiu(reg::sp, reg::sp, -8) == 0x27BD'FFF8);
}

TEST_CASE("the encoder encodes ori and lui as an assembler does", "[support]") {
    STATIC_CHECK(ori(reg::t0, reg::t0, 0x2000) == 0x3508'2000);
    STATIC_CHECK(ori(reg::at, reg::s7, 0xFFFF) == 0x36E1'FFFF);
    STATIC_CHECK(lui(reg::t0, 0x1F80) == 0x3C08'1F80);
    STATIC_CHECK(lui(reg::ra, 0xFFFF) == 0x3C1F'FFFF);
}

TEST_CASE("the encoder encodes sw, with a signed offset, as an assembler does", "[support]") {
    STATIC_CHECK(sw(reg::t1, 0x20, reg::t0) == 0xAD09'0020);
    STATIC_CHECK(sw(reg::ra, -4, reg::sp) == 0xAFBF'FFFC);
}
