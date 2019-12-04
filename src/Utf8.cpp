/**
 * @file Utf8.cpp
 *
 * This module contains the implementation of the Utf8::Utf8 class.
 */

#include <stddef.h>
#include <Utf8/Utf8.hpp>

namespace {
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
            interger >>= 1;
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
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 12) & 0x0F)) + 0xE0);
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 6) & 0x3F)) + 0x80);
                encoding.push_back((UnicodeCodePoint)(((codePoint) & 0x3F)) + 0x80);
            }
            else if (numBits <= 21) {
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 18) & 0x07)) + 0xF0);
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 12) & 0x3F)) + 0x80);
                encoding.push_back((UnicodeCodePoint)(((codePoint >> 6) & 0x3F)) + 0x80);
                encoding.push_back((UnicodeCodePoint)(((codePoint) & 0x3F)) + 0x80);
            }
            else {
                // to do
            }

        }
        return encoding;
    }
}
