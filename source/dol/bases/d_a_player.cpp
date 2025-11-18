#include <game/bases/d_a_player.hpp>
#include <game/sLib/s_GlobalData.hpp>

const float dAcPy_c::msc_JUMP_SPEED = daPlBase_c::sc_JumpSpeed;

STATE_VIRTUAL_DEFINE(dAcPy_c, Walk);
STATE_VIRTUAL_DEFINE(dAcPy_c, Jump);
STATE_VIRTUAL_DEFINE(dAcPy_c, Fall);
STATE_VIRTUAL_DEFINE(dAcPy_c, Land);
STATE_VIRTUAL_DEFINE(dAcPy_c, Crouch);
STATE_VIRTUAL_DEFINE(dAcPy_c, SitJump);
STATE_VIRTUAL_DEFINE(dAcPy_c, Slip);
STATE_VIRTUAL_DEFINE(dAcPy_c, Turn);
STATE_VIRTUAL_DEFINE(dAcPy_c, HipAttack);
STATE_VIRTUAL_DEFINE(dAcPy_c, Swim);
STATE_VIRTUAL_DEFINE(dAcPy_c, Kani);
STATE_VIRTUAL_DEFINE(dAcPy_c, Cloud);

STATE_DEFINE(dAcPy_c, RideOffJump);
STATE_DEFINE(dAcPy_c, SpinHipAttack);
STATE_DEFINE(dAcPy_c, RollSlip);
STATE_DEFINE(dAcPy_c, Vine);
STATE_DEFINE(dAcPy_c, Hang);
STATE_DEFINE(dAcPy_c, Pole);
STATE_DEFINE(dAcPy_c, Fire);
STATE_DEFINE(dAcPy_c, LiftUp);
STATE_DEFINE(dAcPy_c, Throw);
STATE_DEFINE(dAcPy_c, PropelThrow);
STATE_DEFINE(dAcPy_c, WallJump);
STATE_DEFINE(dAcPy_c, WallSlide);
STATE_DEFINE(dAcPy_c, Propel);
STATE_DEFINE(dAcPy_c, CarryPlayer);
STATE_DEFINE(dAcPy_c, RideYoshi);
STATE_DEFINE(dAcPy_c, SpinJump);
STATE_DEFINE(dAcPy_c, PenguinSlide);
STATE_DEFINE(dAcPy_c, KaniJump);
STATE_DEFINE(dAcPy_c, Quake);
STATE_DEFINE(dAcPy_c, ElecShock);
STATE_DEFINE(dAcPy_c, FlyDamage);
STATE_DEFINE(dAcPy_c, IceDamage);
STATE_DEFINE(dAcPy_c, CannonJump);
STATE_DEFINE(dAcPy_c, TarzanRope);
STATE_DEFINE(dAcPy_c, PlayerEat);
STATE_DEFINE(dAcPy_c, Balloon);
STATE_DEFINE(dAcPy_c, BlockJump);
STATE_DEFINE(dAcPy_c, JrCrown);

STATE_VIRTUAL_DEFINE(dAcPy_c, DemoDown);
STATE_DEFINE(dAcPy_c, DemoInDoor);
STATE_DEFINE(dAcPy_c, DemoInJump);
STATE_DEFINE(dAcPy_c, DemoInVine);
STATE_DEFINE(dAcPy_c, DemoOutDoor);
STATE_DEFINE(dAcPy_c, DemoFallDown);
STATE_DEFINE(dAcPy_c, DemoFireDown);
STATE_DEFINE(dAcPy_c, DemoEatDie);
STATE_DEFINE(dAcPy_c, DemoDokanCannon);
STATE_DEFINE(dAcPy_c, DemoCannonWarp);

template <>
dAcPy_c::GlobalData_t sGlobalData_c<dAcPy_c>::mData = {
    0.0f, 0.0f,
    1.5f, 4.5f, 0.5f, 4.0f, 1.4f, 1.0f,
    mVec3_c(0.0f, 10.0f, 0.0f),
    mVec3_c(0.0f, 16.0f, 0.0f),
    mVec3_c(0.0f, 28.0f, 0.0f)
};

dAcPy_HIO_Speed_c dAcPy_c::m_speed_hio[2];
