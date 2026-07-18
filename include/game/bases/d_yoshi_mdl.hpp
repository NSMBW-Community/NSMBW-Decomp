#pragma once

#include <game/bases/d_player_model_base.hpp>

class dYoshiMdl_c : public dPyMdlBase_c {
public:
    dYoshiMdl_c(u8 modelType);

    enum YoshiColor_e {};

    YoshiColor_e getColor() { return mColor; }

    u8 mPad[0x40];
    YoshiColor_e mColor;
    u8 mPad2[0x1b4];
};
