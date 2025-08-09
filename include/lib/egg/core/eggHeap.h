#pragma once

#include <types.h>
#include <lib/egg/core/eggDisposer.h>
#include <revolution/MEM.h>

// [TODO: extend this]

namespace EGG {

class Allocator;
class FrmHeap;

class Heap : Disposer {
public:
    Heap()
        : mFlag(0) {}
    Heap(MEMiHeapHead *heapHead);
    virtual ~Heap();
    // [TODO: this is likely an enum]
    virtual int getHeapKind() const = 0;
    virtual void initAllocator(Allocator *allocator, long align) = 0;
    virtual void *alloc(size_t, long) = 0;
    virtual void free(void *) = 0;
    virtual void destroy() = 0;

    static void *alloc(size_t, int, EGG::Heap *);
    static void free(void *, EGG::Heap *);

    static Heap *findContainHeap(const void *);

    void appendDisposer(Disposer *disposer) { nw4r::ut::List_Append(&mChildren, disposer); }
    void removeDisposer(Disposer *disposer) { nw4r::ut::List_Remove(&mChildren, disposer); }

    MEMiHeapHead *mHeapHandle;
    void *mHeapBuffer;
    Heap *mParentHeap;
    u16 mFlag; // [TODO: TBitFlag<u16>]
    u8 mPad1[0xa];
    nw4r::ut::List mChildren;
    const char *mpName;

    static FrmHeap *dynamicCastToFrm(Heap *heap) {
        if (heap != nullptr && heap->getHeapKind() == 2) {
            return (FrmHeap *)heap;
        }
        return nullptr;
    }

    static Heap *sCurrentHeap;
};

} // namespace EGG

void *operator new(size_t, void *);
void *operator new(size_t, int);
void *operator new(size_t, EGG::Heap *, int);
void *operator new[](size_t, EGG::Heap *, int);
