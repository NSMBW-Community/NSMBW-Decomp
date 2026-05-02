#pragma once

#include <types.h>
#include <game/mLib/m_angle.hpp>

class dWmBgmSync_c {
public:
    dWmBgmSync_c() :
        m_04(0), m_08(0),
        m_0c(false), m_0d(false), m_0e(false),
        m_18(nullptr) {}

    virtual ~dWmBgmSync_c();
    virtual void execute();

    float getAnmRate(float frameCount);

    int m_04;
    int m_08;
    bool m_0c;
    bool m_0d;
    bool m_0e;
    u8 mPad2[0x8];
    const s16 *m_18;
};
