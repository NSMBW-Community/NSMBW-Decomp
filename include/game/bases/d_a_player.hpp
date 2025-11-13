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

    char mPad[0x15e8];
    fBaseID_e mCarryActorID;

    static const float msc_JUMP_SPEED;
};
