#pragma once
#include <types.h>
/// @file

namespace nw4r {
namespace math {

/// @brief Computes the inverse square root of the given value.
float FrSqrt(float x);

/// @brief Computes the square root of the given value.
inline float FSqrt(float x) {
    return (x <= 0) ? 0.0f : x * FrSqrt(x);
}

/// @brief Computes 1 / n efficiently with fres + 1 Newton iteration
inline float FInv(register float n) {
    register float x0, tmp1, tmp2;
    asm {
        // Approximate with fres, then use Newton iteration (https://cr.yp.to/2005-590/powerpc-cwg.pdf 3.3.6.3)
        // https://en.wikipedia.org/wiki/Division_algorithm#Newton%E2%80%93Raphson_division
        // Want to calculate x_1 = 2 * x_0 - n * x_0 ^ 2

        fres x0, n

        ps_add tmp1, x0, x0 // tmp1 = 2 * x_0
        ps_mul tmp2, x0, x0 // tmp2 = x_0 ^ 2
        ps_nmsub x0, n, tmp2, tmp1 // x_1 = -(n * tmp2 - tmp1) = tmp1 - n * tmp2
    }

    return x0;
}

} // namespace math
} // namespace nw4r
