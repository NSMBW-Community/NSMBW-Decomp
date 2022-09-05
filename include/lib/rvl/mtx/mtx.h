#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void PSVECAdd(const VEC3 *v1, const VEC3 *v2, VEC3 *out);
void PSVECSubtract(const VEC3 *v1, const VEC3 *v2, VEC3 *out);
void PSVECScale(const VEC3 *src, VEC3 *dst, f32 scale);

#ifdef __cplusplus
}
#endif
