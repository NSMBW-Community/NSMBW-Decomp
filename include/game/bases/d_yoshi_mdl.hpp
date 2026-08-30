#pragma once

#include <game/bases/d_player_model_base.hpp>

class dYoshiMdl_c : public dPyMdlBase_c {
public:
    enum YoshiColor_e {};

    YoshiColor_e getColor() { return mColor; }

    u8 mPad1[0x40];
    YoshiColor_e mColor;
    u8 mPad2[0x44];
    u32 m_294;
};
