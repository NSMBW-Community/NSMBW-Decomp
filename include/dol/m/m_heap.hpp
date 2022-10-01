#pragma once

#include <lib/egg/heap/frm_heap.hpp>

namespace mHeap {
    void restoreCurrentHeap();
    void destroyFrmHeap(EGG::FrmHeap *);
    unsigned long adjustFrmHeap(EGG::FrmHeap *);

    extern EGG::Heap *g_gameHeaps[3];
};

// [Probably belongs in mHeap]
EGG::FrmHeap *makeFrmAndUpdate(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt); 