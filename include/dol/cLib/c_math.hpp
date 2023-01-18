#pragma once
#include <types.h>
/// @file

/// @brief C Math library.
namespace cM {

    // Conversion utilities
    s16 rad2s(float rad); ///< Converts an angle from radians to units.
    s16 atan2s(float sin, float cos); ///< Converts a sine and a cosine to an angle in units.

    // RNG utilities
    void initRnd(ulong seed); ///< Initializes ::s_rnd with the given seed.
    float rnd(); ///< Generates a floating point number between 0 and 1.
    int rndInt(int max); ///< Generates an integer between 0 and the given max.
    float rndF(float max); ///< Generates a floating point number between 0 and the given max.

    // Float utilities
    /// @brief Gets the minimum float between @p x and @p y .
    inline float fmin(float x, float y) {
        if (x > y) return y;
        else return x;
    }

    /// @brief Gets the maximum float between @p x and @p y .
    inline float fmax(float x, float y) {
        if (x < y) return y;
        else return x;
    }

} // namespace cM
