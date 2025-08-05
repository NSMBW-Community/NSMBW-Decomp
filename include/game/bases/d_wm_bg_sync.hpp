#pragma once

#include <types.h>

class dWmBgmSync_c {
public:
    virtual ~dWmBgmSync_c();
    virtual void execute();

    u8 mPad[4];
    u8 m_00;
    u8 m_01;
    u8 m_02;
    bool m_03;
    bool m_0c;
    bool m_0d;
};
