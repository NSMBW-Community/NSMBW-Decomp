#pragma once
#include <game/bases/d_a_player_base.hpp>

class daYoshi_c : public daPlBase_c {
public:
    u8 mPad[0xa0];
    int m_a0;

    void getTongueTipMtx(mMtx_c *mtx);
    void getMouthMtx(mMtx_c *mtx);
};
