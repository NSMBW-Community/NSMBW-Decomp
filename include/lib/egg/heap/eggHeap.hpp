#pragma once
#include <lib/rvl/mem/MEMHeapCommon.h>
#include <lib/egg/eggDisposer.hpp>

// [TODO: extend this]

namespace EGG {
    class Allocator;
    class FrmHeap;

    class Heap : Disposer {
    public:
        Heap() : mFlag(0) {}
        Heap(MEMiHeapHead *heapHead);
        virtual ~Heap();
        virtual int getHeapKind() const = 0;
        virtual void initAllocator(Allocator* allocator, long align) = 0;
        virtual void *alloc(size_t, long) = 0;
        virtual void free(void *) = 0;
        virtual void destroy() = 0;

        static void *alloc(size_t, int, EGG::Heap *);
        static void free(void *, EGG::Heap *);

        char mPad1[0x8];
        MEMiHeapHead *mpHeapHandle;
        void *mpParentBlock;
        Heap *mpParentHeap;
        u16 mFlag;
        u8 mPad2[0x18];

        FrmHeap *toFrmHeap() {
            if (this != nullptr && getHeapKind() == 2) {
                return (FrmHeap *) this;
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
