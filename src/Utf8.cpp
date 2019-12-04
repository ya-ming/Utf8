/**
 * @file Utf8.cpp
 *
 * This module contains the implementation of the Utf8::Utf8 class.
 */

#include <Utf8/Utf8.hpp>

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

    std::vector<uint8_t> Utf8::Encode(const std::vector<UnicodeCodePoint>& codePoints) {
        std::vector<uint8_t> encoding;
        for (auto codePoint : codePoints) {
            encoding.push_back((UnicodeCodePoint)(codePoint & 0x7F));
        }
        return encoding;
    }
}
