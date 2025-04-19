#pragma once
#include <types.h>
#include <lib/rvl/mem/MEMList.h>

#ifdef __cplusplus
extern "C" {
#endif

struct OSMutex; // TODO: fill in struct

typedef struct {
    u32 mTag;
    MEMLink mpLink;
    MEMList mpList;
    void *mpHeapStart;
    void *mpHeapEnd;
    OSMutex *mpMutex;
} MEMiHeapHead;

struct _MEMAllocatorFuncs;
typedef struct {
    const struct _MEMAllocatorFuncs *mpFuncs;
    void *mpHeapHandle;
    u32 mAlignment;
    u32 mUnused;
} MEMAllocator;

typedef struct _MEMAllocatorFuncs {
    void *(*alloc)(MEMAllocator *, size_t);
    void (*free)(MEMAllocator *, void *);
} MEMAllocatorFuncs;

void *MEMAllocFromAllocator(MEMAllocator *, int);

#ifdef __cplusplus
}
#endif
