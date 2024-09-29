#pragma once
#include <types.h>

namespace nw4r {
namespace ut {

    /// @brief Gets the smaller of the two given values.
    template <typename T>
    inline T Min(T a, T b) {
        return (a > b) ? b : a;
    }

    /// @brief Gets the larger of the two given values.
    template <typename T>
    inline T Max(T a, T b) {
        return (a < b) ? b : a;
    }

} // namespace ut
} // namespace nw4r
