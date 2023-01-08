#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

typedef float Mtx[3][4]; ///< A 3x4 matrix.
typedef float Mtx44[4][4]; ///< A 4x4 matrix.

void PSMTXConcat(const Mtx *a, const Mtx *b, Mtx *out); ///< Concatenates two matrices.
void PSMTXTrans(Mtx *mtx, float x, float y, float z); ///< Sets a translation matrix with the given components.

#ifdef __cplusplus
}
#endif
