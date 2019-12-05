/**
 * @file Utf8.cpp
 *
 * This module contains the implementation of the Utf8::Utf8 class.
 */

#include <stddef.h>
#include <Utf8/Utf8.hpp>

namespace {
    /**
     * This is the Unicode replacement character (�) encoded as UTF-8.
     */
    const std::vector< uint8_t> UTF8_ENCODED_REPLACEMENT_CHARACTER = { 0xEF, 0xBF, 0xBD };

    /**
     * This is the Unicode replacement character (�) as a code point.
     */
    const Utf8::UnicodeCodePoint REPLACEMENT_CHARACTER = 0xFFFD;

    /**
     * Since RFC 3629 (November 2003), the high and low surrogate halves
     * used by UTF-16 (U+D800 through U+DFFF) and code points not encodable
     * by UTF-16 (those after U+10FFFF) are not legal Unicode values, and
     * their UTF-8 encoding must be treated as an invalid byte sequence.
     */
    const Utf8::UnicodeCodePoint FIRST_SURROGATE = 0xD800;
    const Utf8::UnicodeCodePoint LAST_SURROGATE = 0xDFFF;

    /**
     * This is the last code point in the Unicode that is legal.
     */
    const Utf8::UnicodeCodePoint LAST_LEGAL_UNICODE_POINT = 0x10FFFF;

    /**
     * This computes the logarithm (base 2) of the given integer.
     *
     * @param[in] integer
     *      This is the integer for which to compute the logarithm.
     *
     * @return
     *      The logarithm (base 2) of the given integer is returned.
     */
    template< typename I > size_t log2n(I integer) {
        size_t answer = 0;
        while (integer > 0)
        {
            ++answer;
            integer >>= 1;
        }

        return answer;
    }
}

namespace Utf8 {
    std::vector<UnicodeCodePoint> AsciiToUnicode(const std::string& ascii) {
        return std::vector<UnicodeCodePoint>(ascii.begin(), ascii.end());
    }

    /**
     * This contains the private properties of a Utf8 instance.
     */
    struct Utf8::Impl {
        /**
         * This is where we keep the current character that
         * is being decoded.
         */
        UnicodeCodePoint currentCharacterBeingDecoded = 0;

        /**
         * This is the number of input bytes that we will
         * need to read in before we can fully assemble
         * the current character that is being decoded.
         */
        size_t numbytesRemainingToDecode = 0;

        /**
         * This is the number of bytes total that make
         * up the current character being decoded.
         */
        size_t bytesTotalToDecodeCurrentCharacter = 0;
    };

    Utf8::~Utf8() = default;

    Utf8::Utf8()
        : impl_(new Impl) {

    }

    /*
        Char. number range | UTF-8 octet sequence
        (hexadecimal) | (binary)
        --------------------+---------------------------------------------
        0000 0000-0000 007F | 0xxxxxxx
        0000 0080-0000 07FF | 110xxxxx 10xxxxxx
        0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
        0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    */
    std::vector<uint8_t> Utf8::Encode(const std::vector<UnicodeCodePoint>& codePoints) {
        std::vector<uint8_t> encoding;
        for (auto codePoint : codePoints) {
            const auto numBits = log2n(codePoint);

            if (numBits <= 7) {
                encoding.push_back((UnicodeCodePoint)(codePoint & 0x7F));
            }
            else if (numBits <= 11) {
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 6) & 0x1F)) + 0xC0);
                encoding.push_back((UnicodeCodePoint)(((codePoint) & 0x3F)) + 0x80);
            }
            else if (numBits <= 16) {
                if (
                    (codePoint >= FIRST_SURROGATE)
                    && (codePoint <= LAST_SURROGATE)
                    ) {
                    (void)encoding.insert(
                        encoding.end(),
                        UTF8_ENCODED_REPLACEMENT_CHARACTER.begin(),
                        UTF8_ENCODED_REPLACEMENT_CHARACTER.end()
                        );
                }
                else {
                    encoding.push_back((UnicodeCodePoint)(((codePoint >> 12) & 0x0F)) + 0xE0);
                    encoding.push_back((UnicodeCodePoint)(((codePoint >> 6) & 0x3F)) + 0x80);
                    encoding.push_back((UnicodeCodePoint)(((codePoint) & 0x3F)) + 0x80);
                }
            }
            else if (numBits <= 21 && codePoint <= LAST_LEGAL_UNICODE_POINT) {
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 18) & 0x07)) + 0xF0);
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 12) & 0x3F)) + 0x80);
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 6) & 0x3F)) + 0x80);
                encoding.push_back((UnicodeCodePoint)(((codePoint) & 0x3F)) + 0x80);
            }
            else {
                (void)encoding.insert(
                    encoding.end(),
                    UTF8_ENCODED_REPLACEMENT_CHARACTER.begin(),
                    UTF8_ENCODED_REPLACEMENT_CHARACTER.end()
                );
            }

        }
        return encoding;
    }

    std::vector<UnicodeCodePoint> Utf8::Decode(const std::vector<uint8_t>& encoding) {
        std::vector<UnicodeCodePoint> output;

        for (auto octet : encoding) {
            // Determine the number of bytes based on the n higher-order bits set to 1.
            if (impl_->numbytesRemainingToDecode == 0) {
                if ((octet & 0x80) == 0) {
                    output.push_back(octet);
                }
                else if ((octet & 0xE0) == 0xC0) {
                    impl_->numbytesRemainingToDecode = 1;
                    impl_->currentCharacterBeingDecoded = (octet) & 0x1F;
                }
                else if ((octet & 0xF0) == 0xE0) {
                    impl_->numbytesRemainingToDecode = 2;
                    impl_->currentCharacterBeingDecoded = (octet) & 0x0F;
                }
                else if ((octet & 0xF8) == 0xF0) {
                    impl_->numbytesRemainingToDecode = 3;
                    impl_->currentCharacterBeingDecoded = (octet) & 0x07;
                }
                else {
                    output.push_back(REPLACEMENT_CHARACTER);
                }
                impl_->bytesTotalToDecodeCurrentCharacter = impl_->numbytesRemainingToDecode + 1;
            }
            // The following bytes must start with '10xxxxxx',
            // if not fill output with replacement character.
            else if ((octet & 0xC0) != 0x80) {
                output.push_back(REPLACEMENT_CHARACTER);
                impl_->numbytesRemainingToDecode = 0;
                const auto nextCodePoints = Decode(std::vector<uint8_t> {octet});
                output.insert(output.end(), nextCodePoints.begin(), nextCodePoints.end());
            }
            else {
                impl_->currentCharacterBeingDecoded <<= 6;
                impl_->currentCharacterBeingDecoded += (octet & 0x3F);
                if (--impl_->numbytesRemainingToDecode == 0) {
                    // Make sure the value of the decoded bytes matches the definition
                    if (
                        (
                        (impl_->bytesTotalToDecodeCurrentCharacter >= 2)
                            && (impl_->currentCharacterBeingDecoded < 0x0080)
                            )
                        || (
                        (impl_->bytesTotalToDecodeCurrentCharacter >= 3)
                            && (impl_->currentCharacterBeingDecoded < 0x0800)
                            )
                        || (
                        (impl_->bytesTotalToDecodeCurrentCharacter >= 4)
                            && (impl_->currentCharacterBeingDecoded < 0x10000)
                            )
                        ) {
                        output.push_back(REPLACEMENT_CHARACTER);
                    }
                    else {
                        output.push_back(impl_->currentCharacterBeingDecoded);
                    }
                    impl_->currentCharacterBeingDecoded = 0;
                }
            }
        }

        return output;
    }

    std::vector< UnicodeCodePoint > Utf8::Decode(const std::string& encoding) {
        return Decode(
            std::vector< uint8_t >(
                encoding.begin(),
                encoding.end()
                )
        );
    }
}
