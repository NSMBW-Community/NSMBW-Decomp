#pragma once

#include <types.h>

class dPlayerOrchestra_c {
public:
    dPlayerOrchestra_c();

    void init(int playerNo);
    void update();

    u8 mPad[0x20];
};
