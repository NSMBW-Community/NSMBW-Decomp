#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *mpPrev;
    void *mpNext;
} MEMLink;

typedef struct {
    void *mpHead;
    void *mpTail;
    u16 mCount;
    u16 mOffset;
} MEMList;

#ifdef __cplusplus
}
#endif
