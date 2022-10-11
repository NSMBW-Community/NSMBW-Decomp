#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void PSVECAdd(const Vec *v1, const Vec *v2, Vec *out);
void PSVECSubtract(const Vec *v1, const Vec *v2, Vec *out);
void PSVECScale(const Vec *src, Vec *dst, f32 scale);

#ifdef __cplusplus
}
#endif
