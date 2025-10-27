#pragma once

#include <types.h>
#include <nw4r/ut.h>

class dFontMng_c {
public:
    static void *getResFontData(const char *name);
    static u8 getResFontIndex(const char *name);
    static nw4r::ut::Font *getFont(int index);
};
