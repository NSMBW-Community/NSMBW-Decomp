#pragma once
#include <game/bases/d_a_player_base.hpp>

class dAcPy_c : public daPlBase_c {
public:
    bool isDrawingCarryFukidashi();
    void getCcBounds(sRangeDataF &bounds); ///< @unofficial
    void cancelCarry(dActor_c *carriedActor);

    mVec3_c getCarryPos();
    mMtx_c getCarryMtx() {
        mMtx_c mtx;
        mtx.trans(getCarryPos());
        mtx.concat(getModel()->getMtx());
        return mtx;
    }

    bool FUN_8012e540(dActor_c *, bool); ///< @unofficial

    u8 mPad1[0x44];
    u32 m_44;
    u8 mPad2[0x54];
    mVec3_c mBgRelatedPos;
    u8 mPad3[0x4];
    s8 mScrollMode;
    u8 mPad4[0x1538];
    fBaseID_e mCarryActorID;
    s8 mPowerup;

    static const float msc_JUMP_SPEED;
};
