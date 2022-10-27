#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

typedef float Mtx[3][4]; ///< 3x4 matrix.
typedef float Mtx44[4][4]; ///< 4x4 matrix.

void PSMTXConcat(const Mtx *a, const Mtx *b, Mtx *out); ///< Concatenates two matrices.

#ifdef __cplusplus
}
#endif
