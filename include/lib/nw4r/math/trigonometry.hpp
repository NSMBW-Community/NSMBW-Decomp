#pragma once
#include <types.h>
#include <lib/rvl/os/OSFastCast.h>
/// @file

namespace nw4r {
namespace math {

/// @brief Computes the sine value.
/// @param fidx The angle, measured in units of 1/256th of a circle.
/// @return The sine for fidx.
float SinFIdx(float fidx);

/// @brief Computes the cosine value.
/// @param fidx The angle, measured in units of 1/256th of a circle.
/// @return The cosine for fidx.
float CosFIdx(float fidx);

/// @brief Computes the sine value.
/// @param ang The angle, measured in units of 1/65536th of a circle.
/// @return The sine for idx.
inline float SinF(float ang) {
    return SinFIdx(ang * (1.0f / 256.f));
}

/// @brief Computes the cosine value.
/// @param ang The angle measured in units of 1/65536th of a circle.
/// @return The cosine for idx.
inline float CosF(float ang) {
    return CosFIdx(ang * (1.0f / 256.f));
}

/// @brief Computes the sine value.
/// @param ang The angle, measured in units of 1/65536th of a circle.
/// @return The sine for idx.
inline float SinS(short ang) {
    return SinF(OSu16tof32((u16 *) &ang));
}

/// @brief Computes the cosine value.
/// @param ang The angle measured in units of 1/65536th of a circle.
/// @return The cosine for idx.
inline float CosS(short ang) {
    return CosF(OSu16tof32((u16 *) &ang));
}

} // namespace math
} // namespace nw4r
