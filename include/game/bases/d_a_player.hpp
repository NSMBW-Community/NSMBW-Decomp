#pragma once
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_hio.hpp>
#include <game/bases/d_player_orchestra.hpp>
#include <game/bases/d_propel_parts.hpp>
#include <game/bases/d_pc.hpp>

class dAcPy_c : public daPlBase_c {
public:
    /// @unofficial
    enum SpinHipAttackSubstate_e {
        SPIN_HIP_ATTACK_ACTION_0,
        SPIN_HIP_ATTACK_ACTION_1
    };

    /// @unofficial
    enum FallSubstate_e {
        FALL_ACTION_0,
        FALL_ACTION_1
    };

    /// @unofficial
    enum JumpSubstate_e {
        JUMP_TAKE_OFF,
        JUMP_AIR
    };

    /// @unofficial
    enum LandSubstate_e {
        LAND_ACTION_0,
        LAND_ACTION_1
    };

    /// @unofficial
    enum SpinJumpSubstate_e {
        SPIN_JUMP_ACTION_0,
        SPIN_JUMP_ACTION_1
    };

    /// @unofficial
    enum SitJumpSubstate_e {
        SIT_JUMP_ACTION_0,
        SIT_JUMP_ACTION_1
    };

    /// @unofficial
    enum CannonJumpSubstate_e {
        CANNON_JUMP_ACTION_0,
        CANNON_JUMP_ACTION_1,
        CANNON_JUMP_ACTION_2
    };

    /// @unofficial
    enum BlockJumpSubstate_e {
        BLOCK_JUMP_ACTION_0,
        BLOCK_JUMP_ACTION_1
    };

    /// @unofficial
    enum RollSlipSubstate_e {
        ROLL_SLIP_ACTION_0,
        ROLL_SLIP_ACTION_1,
        ROLL_SLIP_ACTION_2
    };

    /// @unofficial
    enum PenguinSlideSubstate_e {
        PENGUIN_SLIDE_ACTION_0,
        PENGUIN_SLIDE_ACTION_1,
        PENGUIN_SLIDE_ACTION_2
    };

    /// @unofficial
    enum CrouchSubstate_e {
        CROUCH_GROUND,
        CROUCH_WATER
    };

    struct GlobalData_t {
        float f1, f2, f3, f4, f5, f6, f7, f8;
        mVec3_c mPos[3];
    };

    dAcPy_c();
    virtual ~dAcPy_c();

    virtual bool setHipAttackOnEnemy(mVec3_c *hitPos);
    virtual void setHipAttackEffect();
    virtual void setHipAttack_AttackStart();
    virtual void setSlipAction();
    virtual void slipActionMove(int);
    virtual bool isWaitFrameCountMax();
    virtual bool checkWalkNextAction();
    virtual void setWaitActionAnm(AnmBlend_e);
    virtual void setWalkActionAnm(AnmBlend_e);
    virtual void walkActionInit_Wait(AnmBlend_e);
    virtual void walkAction_Wait();
    virtual void walkActionInit_Move(AnmBlend_e);
    virtual void walkAction_Move();
    virtual bool checkCrouch();
    virtual bool setCancelCrouch();
    virtual bool isCarry() const { return mCarryActorID != BASE_ID_NULL; }

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

    virtual bool isCarryMamePlayer();

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

    void setSeaLandSmokeEffect();
    bool setHipAttackAction();
    void setWaterInEffect();
    void setSpinHipAttackEffect();
    void setScrollMode(s8);
    void setCcAtSpinFall();
    dActor_c *getCarryPropelActor();
    void setPropelSpinSmokeEffect();
    void setWaterWalkFlag();
    bool setKaniActionInitHangHand();
    bool jump_common();
    void _jumpSet(jmpInf_c *);
    void calcJumpCount();
    float getJumpSpeed();
    void setJumpSpeed();
    void fn_80127740(int, int); ///< Jump animation set, some enum as first param @unofficial
    void fn_80145fd0(int); ///< Jump voice, some enum as param @unofficial
    bool checkCarryThrow();
    void jumpExeTakeOff();
    void jumpExecAir();
    void fn_801282d0(int); ///< @unofficial
    bool fn_8012e540(dActor_c *, bool); ///< @unofficial
    void setJumpCommonBase();
    dAcPy_c *getCarryPlayer();
    bool isIceSlipAnmPlay();
    int checkWallSlideEnable(int);
    bool checkWallJump();
    void setWallSlideEffect();
    void setSpinHoldReq(float);
    bool setSpinSpeed(float, float);
    void setSpinJumpEffect(int);
    void endSpinJumpEffect();
    void setSpinJumpEffectSub(int);
    void setMissSpinJumpEffect(int);
    void initRideSpinMove();
    void setCcAtSpin();
    void setSpinFireBall();
    void endRideSpinMove();
    bool isMissSpinEnableAnm();
    s16 getMissSpinAngle();
    void setMissSpin();
    void resetMissSpin();
    void updateMissSpin();
    void setKaniMoveSpeed(bool);
    void setCrouchSmokeEffect();
    void setSlipSE();
    void setSlipEffect();
    void releaseCarryActor();
    void setInvalidKeyTimer_LR(int, int);
    bool checkPenguinSlideJump();
    void setPenguinSlideEffect();
    void setPenguinSlideLandEffect();
    void setNormalWalkAnm(AnmBlend_e, float);
    void setRunOnWaterEffect();
    void turnPowerSet();
    void setTurnSmokeEffect();
    void CrouchActionGround();
    void CrouchActionWater();
    void setWaterGroundJump();
    dActor_c *getCarryPropelBlock();
    dActor_c *getCarryHardBlock();
    bool isLiftUp();
    bool isLiftUpExceptMame();
    float getLiftUpOffset();
    mVec3_c getLiftUpPos();
    void clearSpinLiftUpReserve();
    void checkSpinLiftUpReserve(dCc_c *cc);
    void setSpinLiftUpReserve();
    void checkSpinLiftUpRoofHeight();
    void setCarryOffFall(const dAcPy_c *player);

    bool isDrawingCarryFukidashi();
    void getCcBounds(sRangeDataF &bounds); ///< @unofficial
    bool cancelCarry(dActor_c *carriedActor);

    mVec3_c getCarryPos();
    mMtx_c getCarryMtx() {
        mMtx_c mtx;
        mtx.trans(getCarryPos());
        mtx.concat(getModel()->getMtx());
        return mtx;
    }

    int mCreateItemRelated;
    u8 mPad1[0x20];
    dPc_c mPc;
    u8 mPad2[0x10];
    int m_68;
    u8 mPad3[0x14];
    int m_80;
    int mWaitFrameCount;
    int m_88;
    int mJumpCounter;
    u8 m_90;
    u8 m_91;
    float m_94;
    mEf::levelEffect_c mLevelEf1;
    mEf::levelEffect_c mLevelEf2;
    mAng m_2e8;
    u8 mPad5[0x10];
    mEf::levelEffect_c mLevelEf3;
    dEf::followEffect_c mFollowEf;
    u8 mPad6[0x8];
    int m_540;
    int m_544;
    int m_548;
    float m_54c;
    mEf::levelEffect_c mLevelEf4;
    mEf::levelEffect_c mLevelEf5;
    u8 mPad7[0x28];
    mEf::levelEffect_c mLevelEf6;
    u8 mPad8[0x30];
    mEf::levelEffect_c mLevelEf7;
    mEf::levelEffect_c mLevelEf8;
    u8 mPad9[0x28];
    int m_b98;
    mEf::levelEffect_c mLevelEf9;
    mEf::levelEffect_c mLevelEf10;
    mEf::levelEffect_c mLevelEf11;
    mEf::levelEffect_c mLevelEf12;
    u8 mPad10[0x8];
    int m_1044;
    s16 m_1048;
    u8 mPad11[0x4];
    float mSpinHoldReq;
    u32 m_1054;
    u32 m_1058;
    s16 m_105c;
    dEf::followEffect_c mFollowEf2;
    dEf::followEffect_c mFollowEf3;
    u32 m_1288;
    u32 m_128c;
    u8 mPad12[0x18];
    mVec3_c m_12a8;
    u32 m_12b4;
    u8 mPad13[0x3c];
    u8 m_12f4;
    float m_12f8;
    fBaseID_e m_12fc;
    float m_1300;
    float m_1304;
    int m_1308;
    u8 mPad15[0x20];
    dEf::dLevelEffect_c mLevelEf14;
    dEf::dLevelEffect_c mLevelEf15;
    u8 mPad16[0xc];
    dPyMdlMng_c mPyMdlMng;
    u8 mPad17[0x4];
    float m_1598;
    u8 mPad18[0x4];
    fBaseID_e mCarryActorID;
    u8 mPad19[0x8];
    short m_15ac;
    short m_15ae;
    u16 m_15b0;
    u16 m_15b2;
    s16 m_15b4;
    u16 m_15b6;
    u8 mPad20[0x4];
    dEf::followEffect_c mFollowEf4;
    dPropelParts_c mPropelParts;
    dPlayerOrchestra_c mPlayerOrchestra;

    ACTOR_PARAM_CONFIG(PlayerNo, 0, 4);

    static dAcPy_HIO_Speed_c m_speed_hio[2];
    static const float msc_JUMP_SPEED;
};
