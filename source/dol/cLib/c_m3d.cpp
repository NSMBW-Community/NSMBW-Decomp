#include <types.h>
#include <lib/rvl/mtx/mtx.h>

void cM3d_calcInDivPos1(const VEC3 *pA, const VEC3 *pB, float scale, VEC3 *pDest) {
    VEC3 scaledVec;
    PSVECScale(pB, &scaledVec, scale);
    PSVECAdd(&scaledVec, pA, pDest);
}

void cM3d_calcInDivPos2(const VEC3 *pA, const VEC3 *pB, float scale, VEC3 *pDest) {
    VEC3 dist;
    PSVECSubtract(pB, pA, &dist);
    cM3d_calcInDivPos1(pA, &dist, scale, pDest);
}
