#pragma once

#include <lib/egg/core/eggHeap.h>

namespace EGG {

class TextureBuffer {
public:
    static void initialize(ulong size, Heap *heap);
};

} // namespace EGG
