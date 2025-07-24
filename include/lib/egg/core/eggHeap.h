#pragma once
#include <lib/rvl/mem/MEMHeapCommon.h>
#include <lib/egg/core/eggDisposer.h>

// [TODO: extend this]

namespace EGG {
    class Allocator;
    class FrmHeap;

    class Heap : Disposer {
    public:
        Heap() : mFlag(0) {}
        Heap(MEMiHeapHead *heapHead);
        virtual ~Heap();
        // [TODO: this is likely an enum]
        virtual int getHeapKind() const = 0;
        virtual void initAllocator(Allocator* allocator, long align) = 0;
        virtual void *alloc(size_t, long) = 0;
        virtual void free(void *) = 0;
        virtual void destroy() = 0;

        static void *alloc(size_t, int, EGG::Heap *);
        static void free(void *, EGG::Heap *);

        char mPad1[0x8]; // [TODO: Remove this padding when Disposer's link is added]
        MEMiHeapHead *mHeapHandle;
        void *mHeapBuffer;
        Heap *mParentHeap;
        u16 mFlag; // [TODO: TBitFlag<u16>]
        u8 mPad2[0x18];

        static FrmHeap *dynamicCastToFrm(Heap *heap) {
            if (heap != nullptr && heap->getHeapKind() == 2) {
                return (FrmHeap *) heap;
            }
            return nullptr;
        }

        static Heap *sCurrentHeap;
    };
}

void *operator new(size_t, void *);
void *operator new(size_t, int);
void *operator new(size_t, EGG::Heap *, int);
void *operator new[](size_t, EGG::Heap *, int);
