#pragma once
#include <game/bases/d_a_player_base.hpp>

class dAcPy_c : public daPlBase_c {
public:
    bool isDrawingCarryFukidashi();
    void getCcBounds(mBoundBox &bounds); ///< @unofficial
    void cancelCarry(dActor_c *carriedActor);

    char mPad[0x15e8];
    fBaseID_e mCarryActorID;
};
