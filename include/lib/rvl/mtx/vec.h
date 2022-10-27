#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Two-dimensional floating point vector.
typedef struct {
    float x, y;
} Vec2;

/// @brief Three-dimensional floating point vector.
typedef struct {
    float x, y, z;
} Vec;

/// @brief Three-dimensional short vector.
typedef struct {
    s16 x, y, z;
} S16Vec;

void PSVECAdd(const Vec *v1, const Vec *v2, Vec *out); ///< Sums two vectors together.
void PSVECSubtract(const Vec *v1, const Vec *v2, Vec *out); ///< Subtracts vector @p v2 from vector @p v1.
void PSVECScale(const Vec *in, Vec *out, float scale); ///< Multiplies a vector by a scalar.

#ifdef __cplusplus
}
#endif
