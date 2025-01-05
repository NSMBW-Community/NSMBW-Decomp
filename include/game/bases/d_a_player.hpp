#pragma once
#include <game/bases/d_a_player_base.hpp>

class dAcPy_c : public daPlBase_c {
public:
    bool isDrawingCarryFukidashi();
    void getCcBounds(AreaBound &);
    void cancelCarry(dActor_c *);
    mVec3_c getCarryPos();
    bool FUN_8012e540(dActor_c *, bool);

    char pad[0x15e8];
    fBaseID_e carryActorID;

    static const float msc_JUMP_SPEED;
};
