/**
 * @file Utf8Testss.cpp
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

TEST(Utf8Tests, StumpOfTree) {
    Utf8::Utf8 utf8;
    const std::vector< uint8_t > expectedEncoding{ 0xF0, 0xA3, 0x8E, 0xB4 };
    const auto actualEncoding = utf8.Encode({ 0x233B4 }); // 𣎴
    ASSERT_EQ(expectedEncoding, actualEncoding);
}

TEST(Utf8Tests, CodePointBeyondEndOfLastValidRange) {
    Utf8::Utf8 utf8;
    const std::vector< uint8_t > replacementCharacterEncoding{ 0xEF, 0xBF, 0xBD };
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0x200000 }));
    ASSERT_EQ(replacementCharacterEncoding, utf8.Encode({ 0x110000 }));
}

TEST(Utf8Tests, HighAndLowSurrogateHalvesAreInvalid) {
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

TEST(Utf8Tests, DecodeValidSequences) {
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

TEST(Utf8Tests, DecodeFromInputVector) {
    Utf8::Utf8 utf8;
    const auto actualDecoding = utf8.Decode(std::vector< uint8_t >{0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E});
    ASSERT_EQ((std::vector< Utf8::UnicodeCodePoint >{0x65E5, 0x672C, 0x8A9E}), actualDecoding);
}

TEST(Utf8Tests, UnexpectedContinuationBytes) {
    Utf8::Utf8 utf8;
    ASSERT_EQ(
        (std::vector< Utf8::UnicodeCodePoint >{0x0041, 0x2262, 0xFFFD, 0x002E}),
        utf8.Decode(std::vector< uint8_t >{ 0x41, 0xE2, 0x89, 0xA2, 0x91, 0x2E })
    ); // A≢�.
}

TEST(Utf8Tests, DecodeBreakInSequence) {
    Utf8::Utf8 utf8;
    ASSERT_EQ(
        (std::vector< Utf8::UnicodeCodePoint >{0x0041, 0x2262, 0xFFFD, 0x2262}),
        utf8.Decode(std::vector< uint8_t >{ 0x41, 0xE2, 0x89, 0xA2, 0xCE, 0xE2, 0x89, 0xA2 })
    ); // A≢�.
}

TEST(Utf8Tests, RejectOverlongSequences) {
    const std::vector< std::vector< uint8_t > > testVectors{
        // All U+2F ('/') -- should only need 1 byte
        { 0xc0, 0xaf }, // 0xaf == 1010 1111, the first 2 bits are the prefix '10', actual value is
                        // 10 1111 == 0x2f
        { 0xe0, 0x80, 0xaf },
        { 0xf0, 0x80, 0x80, 0xaf },

        // One less than the minimum code point value
        // that should require this many encoded bytes
        { 0xc1, 0xbf }, // U+7F (should be 1 byte)
        { 0xe0, 0x9f, 0xbf }, // U+7FF (should be 2 bytes)
        { 0xf0, 0x8f, 0xbf, 0xbf }, // U+FFFF (should be 3 bytes)
    };
    size_t index = 0;
    for (const auto& testVector : testVectors) {
        Utf8::Utf8 utf8;
        ASSERT_EQ(
            (std::vector< Utf8::UnicodeCodePoint >{0xFFFD}),
            utf8.Decode(testVector)
        ) << index;
        ++index;
    }
}
