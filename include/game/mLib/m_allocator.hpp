#pragma once
#include <types.h>
#include <lib/egg/core/eggAllocator.h>

/**
 * @brief An allocator class that wraps an EGG:Allocator .
 */
class mAllocator_c : public EGG::Allocator {
public:
    mAllocator_c(); ///< @brief Constructs a new allocator.
    virtual ~mAllocator_c(); ///< @brief Destroys the allocator.

    virtual void *alloc(size_t size);
    virtual void free(void *ptr);

    bool attach(EGG::Heap *heap, int alignment);

    static void init(EGG::Heap *heap);
};

/**
 * @brief An allocator with some helper functions that manage the heap
 * from which it allocates memory.
 */
class mHeapAllocator_c : public mAllocator_c {
public:
    mHeapAllocator_c(); ///< @brief Constructs a new heap allocator.
    virtual ~mHeapAllocator_c(); ///< @brief Destroys the heap allocator.

    /// @unofficial
    bool createHeap(size_t size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    /// @unofficial
    bool createHeapRestoreCurrent(size_t size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    void destroyHeap(); ///< @brief destroys the underlying heap and replaces it with the dummy heap;
    size_t adjustFrmHeap();
    size_t adjustFrmHeapRestoreCurrent();
};
