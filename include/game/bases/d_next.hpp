#pragma once
#include <types.h>
#include <game/bases/d_fader.hpp>

class dNext_c {
public:
    bool fn_800cfed0(u8, u8);
    void setChangeSceneNextDat(u8, u8, dFader_c::fader_type_e);

    u8 mPad1[0x19];
    bool m_19;

    static dNext_c *m_instance;
};
