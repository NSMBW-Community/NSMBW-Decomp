#pragma once
#include <types.h>
#include <game/mLib/m_allocator.hpp>
#include <nw4r/ut.h>
#include <lib/egg/core/eggHeap.h>

namespace m2d {
    void reset();
    bool create(EGG::Heap *, size_t);
    void defaultSet();
    void draw();
    void drawBefore(u8 maxDrawOrder);
    void drawAfter(u8 minDrawOrder);
    void drawBtween(u8 minDrawOrder, u8 maxDrawOrder);
    mAllocator_c *getAllocator();

    extern nw4r::ut::List l_list;
    extern mAllocator_c *l_allocator;
} // namespace m2d
