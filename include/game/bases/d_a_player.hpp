#pragma once
#include <game/bases/d_a_player_base.hpp>

class dAcPy_c : public daPlBase_c {
public:
    bool isDrawingCarryFukidashi();
    void getCcBounds(mBoundBox &bounds); ///< @unofficial
    void cancelCarry(dActor_c *carriedActor);

    u8 mPad1[0x60];
    float m_68;
    u8 mPad2[0x38];
    mVec3_c mBgRelatedPos;
    u8 mPad3[0x4];
    s8 mScrollMode;
    u8 mPad4[0x1538];
    fBaseID_e mCarryActorID;

    static const float msc_JUMP_SPEED;
};
