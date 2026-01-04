#pragma once
#include <types.h>
#include <lib/egg/core/eggHeap.h>

/**
 * @brief A non-functional heap to be used when mHeapAllocator_c
 * does not have a proper heap.
 */
class mAllocatorDummyHeap_c : public EGG::Heap {
public:
    mAllocatorDummyHeap_c();
    virtual ~mAllocatorDummyHeap_c();

    virtual int getHeapKind() const;

    virtual void initAllocator(EGG::Allocator *allocator, long alignment);

    virtual void *alloc(size_t bytes, long alignment);
    virtual void free(void *ptr);

    virtual void destroy();
    virtual size_t resizeForMBlock(void *ptr, size_t newSize);

    virtual u32 getTotalFreeSize();
    virtual size_t getAllocatableSize(long alignment);
    virtual size_t adjust();

    static void *AllocatorAllocForDummyHeap(MEMAllocator *, size_t);
    static void AllocatorFreeForDummyHeap(MEMAllocator *, void *);

    static mAllocatorDummyHeap_c *getInstance();

    static mAllocatorDummyHeap_c *m_instance;
};
