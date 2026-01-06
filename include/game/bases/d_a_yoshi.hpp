#pragma once
#include <game/bases/d_a_player_base.hpp>

class daYoshi_c : public daPlBase_c {
public:
    u8 mPad1[0x58];
    int m_a0;
    u8 mPad2[0x30];
    int m_d4;

    void getTongueTipMtx(mMtx_c *mtx);
    void getMouthMtx(mMtx_c *mtx);
    daPlBase_c *getPlayerRideOn() const;
};
