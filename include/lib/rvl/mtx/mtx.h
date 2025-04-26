#pragma once
#include <types.h>
/// @file

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup mtx
/// @{

typedef float Mtx[3][4]; ///< A 3x4 matrix.
typedef float Mtx44[4][4]; ///< A 4x4 matrix.

typedef struct _QUAT {
    float x, y, z, w;
} QUAT;

void PSMTXIdentity(Mtx *mtx); ///< Sets the given matrix to the identity matrix.
void PSMTXConcat(const Mtx *a, const Mtx *b, Mtx *out); ///< Concatenates two matrices.
void PSMTXCopy(const Mtx *src, Mtx *dst); ///< Copies a matrix.
void PSMTXTrans(Mtx *mtx, float x, float y, float z); ///< Sets a translation matrix with the given components.
void PSMTXScale(Mtx *mtx, float x, float y, float z); ///< Sets a scaling matrix with the given components.
void PSMTXQuat(Mtx *mtx, QUAT *);

void C_QUATMtx(QUAT *, Mtx *);
void C_QUATSlerp(QUAT *, QUAT *, QUAT *, float);

void C_MTXOrtho(Mtx44 *, float, float, float, float, float, float);

/// @}

#ifdef __cplusplus
}
#endif
