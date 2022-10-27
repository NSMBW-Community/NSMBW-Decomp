#include <types.h>
#include <lib/rvl/mtx/vec.h>

void cM3d_calcInDivPos1(const Vec *pA, const Vec *pB, float scale, Vec *pDest) {
    Vec scaledVec;
    PSVECScale(pB, &scaledVec, scale);
    PSVECAdd(&scaledVec, pA, pDest);
}

void cM3d_calcInDivPos2(const Vec *pA, const Vec *pB, float scale, Vec *pDest) {
    Vec dist;
    PSVECSubtract(pB, pA, &dist);
    cM3d_calcInDivPos1(pA, &dist, scale, pDest);
}
