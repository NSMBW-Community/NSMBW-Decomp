#pragma once

#include <lib/egg/core/eggHeap.h>

namespace EGG {

class Xfb {
public:
    Xfb(Heap *heap);

    u8 mPad[0x14];
};

} // namespace EGG
