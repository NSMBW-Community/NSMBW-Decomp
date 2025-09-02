#pragma once
#include <types.h>
#include <lib/egg/core/eggHeap.h>

namespace dDvd {

class loader_c {
public:
    loader_c();
    virtual ~loader_c();
    virtual void freeHeap();

    void *request(const char *, u8, EGG::Heap *);
    bool remove();

private:
    u32 mSize;
    u32 mCommand;
    EGG::Heap* mpHeap;
    void* mpBuffer;
};

void create(long, EGG::Heap *, EGG::Heap *);

} // namespace dDvd
