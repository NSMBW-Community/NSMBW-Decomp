#pragma once

#include <types.h>

class dNandThread_c {
public:
    void cmdExistCheck();
    void cmdSpaceCheck();

    u8 mPad[0x74];
    int m_74;
    int m_78;
    bool m_7c;

    static dNandThread_c *m_instance;
};
