#pragma once

// Codewarrior-specific pragmas
#ifdef __CWCC__
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

// Macros
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define BIT_FLAG(bit) ((bit) < 0 ? 0 : 1 << (bit))

#ifdef __CWCC__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif
