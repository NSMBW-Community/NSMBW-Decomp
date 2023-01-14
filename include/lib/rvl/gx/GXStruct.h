#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief A 32-bit RGBA color.
typedef struct {
    union {
        struct {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        };
        u32 rgba;
    };
} GXColor;

#ifdef __cplusplus
}
#endif
