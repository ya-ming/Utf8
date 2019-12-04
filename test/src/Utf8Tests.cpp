/**
 * @file Utf8Tests.cpp
 *
 * This module contains the unit tests of the
 * Utf8::Utf8 class.
 *
 */

#include <stdint.h>
#include <vector>

#include <gtest/gtest.h>
#include <Utf8/Utf8.hpp>

TEST(Utf8Tests, AsciiToUnicdoe) {
    const std::vector<Utf8::UnicodeCodePoint> expectedCodePoint{ 0x48, 0x65, 0x6C, 0x6C, 0x6F };
    const auto actualCodePoints = Utf8::AsciiToUnicode("Hello");
}

TEST(Utf8Tests, EncodeAscii) {
    Utf8::Utf8 utf8;
    const std::vector<uint8_t> expectedCodePoint{ 0x48, 0x65, 0x6C, 0x6C, 0x6F };
    const auto actualEncoding = utf8.Encode(Utf8::AsciiToUnicode("Hello"));

    ASSERT_EQ(expectedCodePoint, actualEncoding);
}
