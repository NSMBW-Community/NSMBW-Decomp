#pragma once

#ifdef __cplusplus
#include <cstdarg>
#include <cstddef>
#include <new>
#else
#include <stdarg.h>
#include <stddef.h>
#endif

// Codewarrior-specific pragmas
#ifdef __CWCC__
#pragma cpp1x on
#pragma gcc_extensions on
#endif

// Base types
typedef unsigned char u8;
typedef unsigned char byte_t;
typedef unsigned short u16;
typedef unsigned short byte2_t;
typedef unsigned int u32;
typedef unsigned int byte4_t;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef float f32;
typedef double f64;
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

typedef int UNKWORD;
typedef void UNKTYPE;

enum { FALSE, TRUE };
typedef int BOOL;

// Macros
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define BIT_FLAG(bit) ((bit) < 0 ? 0 : 1 << (bit))
#define ROUND_UP(x, align) (((x) + (align) - 1) & (-(align)))
#define ROUND_UP_PTR(x, align)  ((void*)((((u32)(x)) + (align) - 1) & (~((align) - 1))))

// No-op on release
#define EGG_ASSERT(...)
#define EGG_ASSERT_MSG(...)

#ifndef NULL
#define NULL 0
#endif // NULL

#ifndef nullptr
#define nullptr 0
#endif // nullptr

#define DECL_SECTION(x) __declspec(section x)
#define DECL_WEAK __declspec(weak)
#define ATTR_UNUSED

#ifdef __CWCC__
#define NOINLINE __attribute__((noinline))
#define ALIGN(x) __attribute__((aligned(x)))
#define DECOMP_INLINE inline
#define DECOMP_DONT_INLINE __attribute__((never_inline))
#define DONT_INLINE __attribute__((never_inline))
#define AT_ADDRESS(x) : x
#define override

// Static assert
#define GLUE(a, b) a##b
#define GLUE2(a, b) GLUE(a, b)
#define STATIC_ASSERT(cond) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#else
#define NOINLINE
#define ALIGN(x)
#define DECOMP_INLINE
#define DECOMP_DONT_INLINE
#define DONT_INLINE
#define AT_ADDRESS(x)
#define STATIC_ASSERT(...)
#endif
