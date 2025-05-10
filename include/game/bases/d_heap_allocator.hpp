#pragma once
#include <game/mLib/m_allocator.hpp>

class dHeapAllocator_c : public mHeapAllocator_c {
public:
    dHeapAllocator_c();
    virtual ~dHeapAllocator_c();

    size_t adjustFrmHeap();
    EGG::FrmHeap *createFrmHeap(size_t size, EGG::Heap *parent, const char *name, size_t align);
};
