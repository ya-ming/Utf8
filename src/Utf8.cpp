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
}
