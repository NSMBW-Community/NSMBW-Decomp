#include <types.h>
#include <lib/rvl/mtx/vec.h>

void cM3d_calcInDivPos1(const Vec *start, const Vec *dir, float scale, Vec *pos) {
    Vec scaledDir;
    PSVECScale(dir, &scaledDir, scale);
    PSVECAdd(&scaledDir, start, pos);
}

void cM3d_calcInDivPos2(const Vec *a, const Vec *b, float scale, Vec *pos) {
    Vec dist;
    PSVECSubtract(b, a, &dist);
    cM3d_calcInDivPos1(a, &dist, scale, pos);
}
