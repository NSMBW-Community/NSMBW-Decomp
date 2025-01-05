#pragma once

#include <types.h>

class dQuake_c {
public:
    u8 pad[0x30];
    u32 mFlags;

    static dQuake_c *m_instance;
};
