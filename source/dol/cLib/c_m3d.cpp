#include <types.h>
#include <lib/rvl/mtx/mtx.h>

void cM3d_calcInDivPos1(const VEC3* v1, const VEC3* v2, float scale, VEC3* dest) {
    VEC3 scaledVec;
    PSVECScale(v2, &scaledVec, scale);
    PSVECAdd(&scaledVec, v1, dest);
}

void cM3d_calcInDivPos2(const VEC3* v1, const VEC3* v2, float scale, VEC3* dest) {
    VEC3 dist;
    PSVECSubtract(v2, v1, &dist);
    cM3d_calcInDivPos1(v1, &dist, scale, dest);
}
