#pragma once

#include <lib/egg/core/eggBitFlag.h>
#include <lib/egg/core/eggDisposer.h>
#include <revolution/MEM.h>

namespace EGG {

class Allocator;
class FrmHeap;

class Heap : Disposer {
public:
    enum EHeapKind {
        HEAP_KIND_NONE,
        HEAP_KIND_EXPAND,
        HEAP_KIND_FRAME,
        HEAP_KIND_UNIT,
        HEAP_KIND_ASSERT
    };

    Heap() {}
    Heap(MEMiHeapHead *heapHead);
    virtual ~Heap();

    virtual EHeapKind getHeapKind() const = 0;
    virtual void initAllocator(Allocator *allocator, long align) = 0;
    virtual void *alloc(size_t, long) = 0;
    virtual void free(void *) = 0;
    virtual void destroy() = 0;
    virtual int resizeForMBlock(void* block, size_t size) = 0;
    virtual u32 getTotalFreeSize();
    virtual size_t getAllocatableSize(long align) = 0;
    virtual size_t adjust() = 0;

    void dump();
    Heap *becomeCurrentHeap();

    void *getStartAddress() {
        return this;
    }
    void *getEndAddress() {
        return mHeapHandle->end;
    }

    void disableAllocation() {
        mFlags.setBit(BIT_DISABLE_ALLOC);
    }
    void enableAllocation() {
        mFlags.resetBit(BIT_DISABLE_ALLOC);
    }

    void setName(const char *name) {
        mpName = name;
    }

    void appendDisposer(Disposer *disposer) { nw4r::ut::List_Append(&mDisposerList, disposer); }
    void removeDisposer(Disposer *disposer) { nw4r::ut::List_Remove(&mDisposerList, disposer); }

    static void *alloc(size_t, int, EGG::Heap *);
    static void free(void *, EGG::Heap *);

    static Heap *findContainHeap(const void *);

    static FrmHeap *dynamicCastToFrm(Heap *heap) {
        if (heap != nullptr && heap->getHeapKind() == HEAP_KIND_FRAME) {
            return (FrmHeap *) heap;
        }
        return nullptr;
    }

    static Heap *getCurrentHeap() {
        return sCurrentHeap;
    }

protected:
    MEMiHeapHead *mHeapHandle;
    void *mHeapBuffer;
    Heap *mParentHeap;

private:
    enum {
        BIT_DISABLE_ALLOC
    };
    TBitFlag<u16> mFlags;

    NW4R_UT_LIST_LINK_DECL();
    nw4r::ut::List mDisposerList;
    const char *mpName;

    static Heap *sCurrentHeap;
};

} // namespace EGG

void *operator new(size_t, void *);
void *operator new(size_t, int);
void *operator new(size_t, EGG::Heap *, int);
void *operator new[](size_t, EGG::Heap *, int);
