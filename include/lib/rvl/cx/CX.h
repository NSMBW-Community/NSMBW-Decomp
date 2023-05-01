#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void CXUncompressLZ(const void *orig, void *dest);
int CXGetUncompressedSize(void *data);

#ifdef __cplusplus
}
#endif
