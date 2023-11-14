#ifndef CLP_STRUCTURED_SEARCH_CLP_SEARCH_TYPE_UTILS_HPP
#define CLP_STRUCTURED_SEARCH_CLP_SEARCH_TYPE_UTILS_HPP

// C++ standard libraries
#include <cstring>
#include <type_traits>

namespace clp_structured {
/**
 * Gets the underlying type of the given enum
 * @tparam T
 * @param enum_member
 * @return The underlying type of the given enum
 */
template <typename T>
constexpr typename std::underlying_type<T>::type enum_to_underlying_type(T enum_member) {
    return static_cast<typename std::underlying_type<T>::type>(enum_member);
}

/**
 * Cast between types by copying the exact bit representation. This avoids
 * issues with strict type aliasing. This method should be removed when we
 * switch to C++20.
 * @tparam Destination
 * @tparam Source
 * @param src
 * @return
 */
template <class Destination, class Source>
std::enable_if_t<
        sizeof(Destination) == sizeof(Source)
                && std::is_trivially_copyable_v<Destination> && std::is_trivially_copyable_v<Source>
                && std::is_trivially_constructible_v<Destination>,
        Destination>
bit_cast(Source const& src) {
    Destination dst;
    std::memcpy(&dst, &src, sizeof(Destination));
    return dst;
}
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_CLP_SEARCH_TYPE_UTILS_HPP
