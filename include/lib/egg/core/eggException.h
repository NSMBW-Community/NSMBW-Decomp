#pragma once

#include <lib/egg/core/eggHeap.h>

namespace EGG {

class Exception {
public:
    static void create(u16, u16, u16, Heap *heap, int);
    static void setPadInfo(const u16 *padInfo);
};

} // namespace EGG
