#pragma once
#include <types.h>
#include <lib/egg/heap/heap.hpp>

namespace d2d {
namespace dDvd {

class loader_c {
public:
    virtual ~loader_c();
    virtual void freeHeap();

private:
    u32 mSize;
    u32 mCommand;
    EGG::Heap* mHeap;
    void* mBuffer;
};

void create(long, EGG::Heap *, EGG::Heap *);


} // namespace dDvd
} // namespace d2d
