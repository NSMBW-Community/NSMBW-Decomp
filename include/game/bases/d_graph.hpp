#pragma once

#include <lib/egg/core/eggHeap.h>

class dGraph_c {
public:
    void painter();

    static void create(EGG::Heap *heap, EGG::Heap *rootHeap1, EGG::Heap *rootHeap2);

    void (*mpPainterFunc)();

    static dGraph_c *ms_Instance;
};
