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

struct OSMutex; // TODO: fill in struct

typedef struct {
    u32 mTag;
    MEMLink mpLink;
    MEMList mpList;
    void *mpHeapStart;
    void *mpHeapEnd;
    OSMutex *mpMutex;
} MEMiHeapHeader;

struct _MEMAllocatorFuncs;
typedef struct {
    struct _MEMAllocatorFuncs *mpFuncs;
    void *mpHeap;
    u32 mHeapParam1;
    u32 mHeapParam2;
} MEMAllocator;

typedef struct _MEMAllocatorFuncs {
    void *(*alloc)(MEMAllocator *, size_t);
    void (*free)(MEMAllocator *, void *);
} MEMAllocatorFuncs;

#ifdef __cplusplus
}
#endif
