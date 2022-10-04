#pragma once

// Codewarrior-specific pragmas
#ifdef __CWCC__
#pragma cpp_extensions on
#pragma cpp1x on
#pragma gcc_extensions on
#endif

// Base types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef float f32;
typedef double f64;
#ifdef __CWCC__
    typedef unsigned long size_t;
#endif
typedef unsigned long ulong;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile f32 vf32;
typedef volatile f64 vf64;

// Common structures

/// @brief 2-dimensional floating point vector.
typedef struct {
    float x, y;
} VEC2;

/// @brief 3-dimensional floating point vector.
typedef struct {
    float x, y, z;
} VEC3;

/// @brief 3-dimensional short vector.
/// @details [This is generally used to represent rotations].
typedef struct {
    s16 x, y, z;
} S16VEC;

typedef float MTX33[3][3];
typedef float MTX34[3][4];
typedef float MTX44[4][4];
