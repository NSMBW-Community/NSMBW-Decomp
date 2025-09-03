#pragma once

#include <types.h>

class dWarningManager_c {
public:
    void AllWarningEnd(bool);

    u8 mPad[0xb00];
    int m_b00;

    static dWarningManager_c *m_instance;
};
