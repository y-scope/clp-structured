#ifndef CLP_STRUCTURED_SEARCH_CLP_SEARCH_TMPUTILS_HPP
#define CLP_STRUCTURED_SEARCH_CLP_SEARCH_TMPUTILS_HPP

#include <string>

#include "src/clp_structured/Utils.hpp"

namespace clp_structured {
/**
 * Checks if the given segment of the stringcould be a multi-digit hex value
 * @param str
 * @param begin_pos
 * @param end_pos
 * @return true if yes, false otherwise
 */
inline bool
could_be_multi_digit_hex_value(std::string const& str, size_t begin_pos, size_t end_pos) {
    if (end_pos - begin_pos < 2) {
        return false;
    }

    for (size_t i = begin_pos; i < end_pos; ++i) {
        auto c = str[i];
        if (!(('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') || ('0' <= c && c <= '9'))) {
            return false;
        }
    }

    return true;
}

/**
 * Checks if character is a delimiter
 * We treat everything except the following quoted characters as a delimiter: "+-./0-9A-Z\a-z"
 * NOTE: For performance, we rely on the ASCII ordering of characters to compare ranges of
 * characters at a time instead of comparing individual characters
 * @param c
 * @return true if c is a delimiter, false otherwise
 */
inline bool is_delim(char c) {
    return !(
            '+' == c || ('-' <= c && c <= '9') || ('A' <= c && c <= 'Z') || '\\' == c || '_' == c
            || ('a' <= c && c <= 'z')
    );
}
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_CLP_SEARCH_TMPUTILS_HPP
