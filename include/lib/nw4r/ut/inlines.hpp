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

    /// @brief Rounds @p x up to a multiple of @p base.
    template <typename T>
    inline T RoundUp(T x, u32 base) {
        return (T)((x + (base - 1)) & ~(base - 1));
    }

} // namespace ut
} // namespace nw4r
