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

TEST(Utf8Tests, Symbols) {
    Utf8::Utf8 utf8;
    std::vector< uint8_t > expectedEncoding{ 0x41, 0xE2, 0x89, 0xA2, 0xCE, 0x91, 0x2E };
    auto actualEncoding = utf8.Encode({ 0x0041, 0x2262, 0x0391, 0x002E }); // A≢Α.
    ASSERT_EQ(expectedEncoding, actualEncoding);

    expectedEncoding = { 0xE2, 0x82, 0xAC };
    actualEncoding = utf8.Encode({ 0x20AC }); // €
    ASSERT_EQ(expectedEncoding, actualEncoding);
}

TEST(Utf8Tests, EncodeJapanese) {
    Utf8::Utf8 utf8;
    const std::vector< uint8_t > expectedEncoding{ 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E };
    const auto actualEncoding = utf8.Encode({ 0x65E5, 0x672C, 0x8A9E }); // 日本語
    ASSERT_EQ(expectedEncoding, actualEncoding);
}

TEST(Utf8Test, StumpOfTree) {
    Utf8::Utf8 utf8;
    const std::vector< uint8_t > expectedEncoding{ 0xF0, 0xA3, 0x8E, 0xB4 };
    const auto actualEncoding = utf8.Encode({ 0x233B4 }); // 𣎴
    ASSERT_EQ(expectedEncoding, actualEncoding);
}

TEST(Utf8Test, CodePointBeyondEndOfLastValidRange) {
    Utf8::Utf8 utf8;
    const std::vector< uint8_t > replacementCharacterEncoding{ 0xEF, 0xBF, 0xBD };
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0x200000 }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0x110000 }));
}

TEST(Utf8Test, HighAndLowSurrogateHalvesAreInvalid) {
    Utf8::Utf8 utf8;
    const std::vector< uint8_t > replacementCharacterEncoding{ 0xEF, 0xBF, 0xBD };
    ASSERT_EQ((std::vector< uint8_t >{0xED, 0x9F, 0xBF}), utf8.Encode({ 0xD7FF }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0xD800 }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0xD801 }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0xD803 }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0xDFEF }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0xDFFE }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0xDFFF }));
    ASSERT_EQ((std::vector< uint8_t >{0xEE, 0x80, 0x80}), utf8.Encode({ 0xE000 }));
}

TEST(Utf8Test, DecodeValidSequences) {
    struct TestVector {
        std::string encoding;
        std::vector< Utf8::UnicodeCodePoint > expectedDecoding;
    };
    const std::vector< TestVector > testVectors{
        { "𣎴",     { 0x233B4 } },
        { "日本語", { 0x65E5, 0x672C, 0x8A9E } },
        { "A≢Α.",   { 0x0041, 0x2262, 0x0391, 0x002E } },
        { "€",      { 0x20AC } },
        { "Hello",  { 0x48, 0x65, 0x6C, 0x6C, 0x6F }},
    };
    for (const auto& testVector : testVectors) {
        Utf8::Utf8 utf8;
        const auto actualDecoding = utf8.Decode(testVector.encoding);
        ASSERT_EQ(testVector.expectedDecoding, actualDecoding);
    }
}

TEST(Utf8Test, DecodeFromInputVector) {
    Utf8::Utf8 utf8;
    const auto actualDecoding = utf8.Decode(std::vector< uint8_t >{0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E});
    ASSERT_EQ((std::vector< Utf8::UnicodeCodePoint >{0x65E5, 0x672C, 0x8A9E}), actualDecoding);
}
