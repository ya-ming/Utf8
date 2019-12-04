#ifndef UTF8_HPP
#define UTF8_HPP

/**
 * @file Utf8.hpp
 *
 * This module declares the Utf8::Utf8 class.
 */

#include <string>
#include <stdint.h>
#include <ostream>
#include <vector>

namespace Utf8 {
    /**
     * This represents a signle character in Unicode
     */
    typedef uint32_t UnicodeCodePoint;

    /**
     * This function is for convenience, converting a given ASCII string
     * into its equivalent sequence of Unicode code points.
     *
     * @param[in] ascii
     *      This is the ASCII string to convert.
     *
     * @return
     *      The Unicode code points for the given ASCII string are returned.
     */
    std::vector<UnicodeCodePoint> AsciiToUnicode(const std::string& ascii);

    /**
     * This class is used to encode or decode Unicode "code points",
     * or characters from many different international character sets,
     * in order to store or transmit them acorss any interface that
     * accepts a sequence of bytes.
     */
    class Utf8 {
        // Lifecycle management
    public:
        ~Utf8();
        Utf8(const Utf8&) = delete;
        Utf8(Utf8&&) = delete;
        Utf8& operator=(const Utf8&) = delete;
        Utf8& operator=(Utf8&&) = delete;

    public:
        /**
         * This is the default constructor.
         */
        Utf8();

        /**
         * This method encodes the given sequence of Unicode code points
         * into UTF-8
         *
         * @param[in] codePoints
         *      These are the Unicode code points to encode.
         *
         * @return
         *      The UTF-8 encoding of the given Unicode code points is returned.
         */
        std::vector<uint8_t> Encode(const std::vector<UnicodeCodePoint>& codePoints);

    private:
        /**
         * This is the type of structure that contains the private
         * properties of the instance.  It is defined in the implementation
         * and declared here to ensure that it is scoped inside the class.
         */
        struct Impl;

        std::unique_ptr<struct Impl> impl_;
    };
}

#endif
