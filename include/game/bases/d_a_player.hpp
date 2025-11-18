#pragma once
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_hio.hpp>

class dAcPy_c : public daPlBase_c {
public:
    struct GlobalData_t {
        float f1, f2, f3, f4, f5, f6, f7, f8;
        mVec3_c mPos1;
        mVec3_c mPos2;
        mVec3_c mPos3;
    };

    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Walk);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Fall);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Land);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Crouch);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, SitJump);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Slip);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, HipAttack);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Swim);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Kani);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Cloud);

    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, DemoDown);

    STATE_FUNC_DECLARE(dAcPy_c, RideOffJump);
    STATE_FUNC_DECLARE(dAcPy_c, SpinHipAttack);
    STATE_FUNC_DECLARE(dAcPy_c, RollSlip);
    STATE_FUNC_DECLARE(dAcPy_c, Vine);
    STATE_FUNC_DECLARE(dAcPy_c, Hang);
    STATE_FUNC_DECLARE(dAcPy_c, Pole);
    STATE_FUNC_DECLARE(dAcPy_c, Fire);
    STATE_FUNC_DECLARE(dAcPy_c, LiftUp);
    STATE_FUNC_DECLARE(dAcPy_c, Throw);
    STATE_FUNC_DECLARE(dAcPy_c, PropelThrow);
    STATE_FUNC_DECLARE(dAcPy_c, WallJump);
    STATE_FUNC_DECLARE(dAcPy_c, WallSlide);
    STATE_FUNC_DECLARE(dAcPy_c, Propel);
    STATE_FUNC_DECLARE(dAcPy_c, CarryPlayer);
    STATE_FUNC_DECLARE(dAcPy_c, RideYoshi);
    STATE_FUNC_DECLARE(dAcPy_c, SpinJump);
    STATE_FUNC_DECLARE(dAcPy_c, PenguinSlide);
    STATE_FUNC_DECLARE(dAcPy_c, KaniJump);
    STATE_FUNC_DECLARE(dAcPy_c, Quake);
    STATE_FUNC_DECLARE(dAcPy_c, ElecShock);
    STATE_FUNC_DECLARE(dAcPy_c, FlyDamage);
    STATE_FUNC_DECLARE(dAcPy_c, IceDamage);
    STATE_FUNC_DECLARE(dAcPy_c, CannonJump);
    STATE_FUNC_DECLARE(dAcPy_c, TarzanRope);
    STATE_FUNC_DECLARE(dAcPy_c, PlayerEat);
    STATE_FUNC_DECLARE(dAcPy_c, Balloon);
    STATE_FUNC_DECLARE(dAcPy_c, BlockJump);
    STATE_FUNC_DECLARE(dAcPy_c, JrCrown);
    STATE_FUNC_DECLARE(dAcPy_c, DemoInDoor);
    STATE_FUNC_DECLARE(dAcPy_c, DemoInJump);
    STATE_FUNC_DECLARE(dAcPy_c, DemoInVine);
    STATE_FUNC_DECLARE(dAcPy_c, DemoOutDoor);
    STATE_FUNC_DECLARE(dAcPy_c, DemoFallDown);
    STATE_FUNC_DECLARE(dAcPy_c, DemoFireDown);
    STATE_FUNC_DECLARE(dAcPy_c, DemoEatDie);
    STATE_FUNC_DECLARE(dAcPy_c, DemoDokanCannon);
    STATE_FUNC_DECLARE(dAcPy_c, DemoCannonWarp);

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

    char mPad[0x15a0];
    fBaseID_e mCarryActorID;

    static dAcPy_HIO_Speed_c m_speed_hio[2];
    static const float msc_JUMP_SPEED;
};
