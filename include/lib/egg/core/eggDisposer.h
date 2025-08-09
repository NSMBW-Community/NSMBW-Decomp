#pragma once

#include <nw4r/ut.h>

namespace EGG {

class Heap;

/**
 * @brief Interface for a garbage collector.
 * @details When a heap is destroyed, its managing memory is implicitly freed.
 * However, the destructors of objects are never explicitly called when this happens.
 * To solve this, heaps have a linked list for classes that can opt-in for explicit destruction.
 *
 * To opt a class in for memory management, privately inherit this class.
 * @ingroup eggcore
 */
class Disposer {
public:
    /// @brief Registers the disposer in the containing heap.
    Disposer();

    /// @brief Unregisters the disposer in the containing heap.
    virtual ~Disposer();

private:
    Heap *mHeap;          ///< Heap in which the disposer is allocated, or NULL otherwise.
    nw4r::ut::Link mLink; ///< Link for the heap's linked list. Managed by the containing heap.
};

} // namespace EGG
