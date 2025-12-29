#pragma once
#include <lib/egg/core/eggAssertHeap.h>
#include <lib/egg/core/eggFrmHeap.h>
#include <lib/egg/core/eggExpHeap.h>
#include <lib/egg/core/eggUnitHeap.h>

namespace mHeap {
    
    enum AllocOptBit_t {
        OPT_NONE = 0,
        OPT_CLEAR_ALLOC = BIT_FLAG(0),
        OPT_DEBUG_FILL = BIT_FLAG(1),
        OPT_THREAD_SAFE = BIT_FLAG(2)
    };

    u16 GetOptFlag(AllocOptBit_t opt);

    EGG::Heap *setCurrentHeap(EGG::Heap *heap);
    void saveCurrentHeap();
    void restoreCurrentHeap();

    size_t expHeapCost(size_t size, ulong align);
    size_t frmHeapCost(size_t size, ulong align);
    size_t untHeapCost(size_t size, ulong count, ulong align);

    void destroyFrmHeap(EGG::FrmHeap *heap);
    size_t adjustFrmHeap(EGG::FrmHeap *heap); 

    EGG::ExpHeap *createExpHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt);
    EGG::FrmHeap *createFrmHeapToCurrent(size_t size, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt);
    EGG::FrmHeap *createFrmHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt);
    EGG::UnitHeap *createUntHeap(size_t size, ulong count, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt);
    EGG::Heap *createHeap(size_t size, EGG::Heap *parent, const char *name);

    EGG::Heap *createGameHeap(int idx, size_t size, EGG::Heap* parent);
    void createGameHeap1(size_t size, EGG::Heap *parent);
    void createGameHeap2(size_t size, EGG::Heap *parent);
    void createArchiveHeap(size_t size, EGG::Heap *parent);
    void createCommandHeap(size_t size, EGG::Heap *parent);
    void createDylinkHeap(size_t size, EGG::Heap *parent);
    EGG::Heap *createAssertHeap(EGG::Heap *parent);

    extern EGG::Heap *g_gameHeaps[3];
};
