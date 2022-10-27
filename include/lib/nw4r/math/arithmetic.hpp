#pragma once
#include <types.h>
/// @file

namespace nw4r {
namespace math {

/// @brief Finds the inverse square root of the given value.
float FrSqrt(float x);

/// @brief Finds the square root of the given value.
inline float FSqrt(float x) {
    return (x <= 0) ? 0.0f : x * FrSqrt(x);
}

} // namespace math
} // namespace nw4r
