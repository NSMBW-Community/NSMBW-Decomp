#pragma once

#include <lib/egg/core/eggHeap.h>

class dDvdErr_c {
public:
    void draw();
    void execute();
    bool isErrorOccured();

    static void createInstance(EGG::Heap *heap);

    static dDvdErr_c *m_pInstance;
};
