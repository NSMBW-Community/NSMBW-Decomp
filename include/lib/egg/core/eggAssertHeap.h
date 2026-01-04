#pragma once
#include <lib/egg/core/eggHeap.h>

namespace EGG {

class AssertHeap : public Heap {
public:
    static AssertHeap *create(size_t size, Heap *parent);
    static size_t getMinSizeForCreate();
};

} // namespace EGG
