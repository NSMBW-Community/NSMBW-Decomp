#pragma once

#include <lib/egg/core/eggSystem.h>

namespace EGG {

class GraphicsFifo {
public:
    static void create(ulong size, EGG::Heap *heap);
};

} // namespace EGG
