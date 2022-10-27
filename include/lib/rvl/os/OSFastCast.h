#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

// The GQRs used for casting
#define OS_FASTCAST_U8 2
#define OS_FASTCAST_U16 3
#define OS_FASTCAST_S8 4
#define OS_FASTCAST_S16 5

// Int to float conversions

/// @brief Converts an unsigned short value into a single-precision floating point value.
inline float OSu16tof32(register u16* in) {
    register f32 r;
    asm { psq_l r, 0(in), 1, OS_FASTCAST_U16 };
    return r;
}

#ifdef __cplusplus
}
#endif
