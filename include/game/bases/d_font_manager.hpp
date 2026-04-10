#pragma once

#include <lib/egg/core/eggHeap.h>
#include <nw4r/ut.h>

class dFontMng_c {
public:
    static bool create(EGG::Heap *heap);

    static void *getResFontData(const char *name);
    static u8 getResFontIndex(const char *name);
    static nw4r::ut::Font *getFont(int index);
};
