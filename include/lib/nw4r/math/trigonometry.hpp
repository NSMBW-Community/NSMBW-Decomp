#pragma once
#include <types.h>
#include <lib/rvl/os/OSFastCast.h>
/// @file

namespace nw4r {
namespace math {

/// @brief Finds the sine value.
/// @param fidx The angle measured in units of 1/256th of a circle.
/// @return The sine for fidx.
float SinFIdx(float fidx);

/// @brief Finds the cosine value.
/// @param fidx The angle measured in units of 1/256th of a circle.
/// @return The cosine for fidx.
float CosFIdx(float fidx);

/// @brief Finds the sine value.
/// @param idx The angle measured in units of 1/65536th of a circle.
/// @return The sine for idx.
inline float SinIdx(s16 idx) {
    return SinFIdx(OSu16tof32((u16*)&idx) * (1.0f / 256.0f));
}

/// @brief Finds the cosine value.
/// @param idx The angle measured in units of 1/65536th of a circle.
/// @return The cosine for idx.
inline float CosIdx(s16 idx) {
    return CosFIdx(OSu16tof32((u16*)&idx) * (1.0f / 256.0f));
}

} // namespace math
} // namespace nw4r
