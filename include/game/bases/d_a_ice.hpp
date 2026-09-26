#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_ice_effect_maker.hpp>
#include <game/bases/d_ice_manager.hpp>

class dIceParam_c {
public:
    static const mVec3_c smc_ICE_DEFSIZE_SQUARE;
    static const mVec3_c smc_ICE_DEFSIZE_TATE;
    static const mVec3_c smc_ICE_DEFSIZE_YOKO;
    static const mVec3_c smc_ICE_DEFSIZE_BIG_SQUARE;
    static const mVec3_c smc_ICE_DEFSIZE_BIG_TATE;
    static const mVec3_c smc_ICE_DEFSIZE_BIG_YOKO;
};

/// @brief Ice blocks for enemies.
class daIce_c : public dActorState_c {
public:
    /// @unofficial
    enum IceType_e {
        ICE_TYPE_0,
        ICE_TYPE_1,
        ICE_TYPE_2,
        ICE_TYPE_3,
        ICE_TYPE_4,
        ICE_TYPE_5,
        ICE_TYPE_6,
        ICE_TYPE_7,
        ICE_TYPE_8
    };

    /// @unofficial
    enum IceWeight_e {
        ICE_WEIGHT_LIGHTEST,
        ICE_WEIGHT_LIGHT,
        ICE_WEIGHT_HEAVY,
        ICE_WEIGHT_HEAVIEST,
        ICE_WEIGHT_COUNT
    };

    typedef void (daIce_c::*EnlargeFunc)();

    daIce_c() : m_5dc(0), m_608(0), m_60c(0), mIceEfMaker(this) {} ///< @copydoc dActorState_c::dActorState_c
    ~daIce_c() {} ///< @copydoc dActorState_c::~dActorState_c

    int create() override;
    int doDelete() override;
    int execute() override;
    int preExecute() override;
    int draw() override;
    void deleteReady() override;
    void finalUpdate() override;
    void block_hit_init() override;
    bool isSpinLiftUpEnable() override { return canCarry(); }
    bool vf68(dBg_ctr_c *collider) override;

    virtual bool someCarryCheck(); ///< @unofficial

    void createMdl();
    void calcMdl();

    void setInfo(mVec3_c size, dIceEfScale_c scale);
    void initSize(const mVec3_c &size);
    void initPos();
    void initWeight();
    void initCollision();
    void initEffect(dIceEfScale_c scale);

    bool checkInstantBreak(int a);
    bool checkPlayerIn();
    bool checkBgIn();
    bool calcLandLine(float &height);

    bool isQuakeDamage();
    void quakeDamage();
    void combobreakReq(int dir, dActor_c *initiator);
    void plcombobreakReq(int dir, dActor_c *initiator);
    void treadcombobreakReq(int dir, dActor_c *initiator);
    void starbreakReq(int dir, dActor_c *initiator);

    void setBc();
    void setCarryBc();
    void enlargeBgc();
    void enlargeBgcLand();
    void enlargeBgcWater();

    void enemyAttach();
    void shake();
    void normalShake();
    void watertogeShake();
    void calcFloatAngle();
    void calcFloatSpeed();

    bool checkPushSink(dActor_c *actor);
    dIceMng_c *getManager();

    bool checkUnderBreak(dActor_c *actor);
    bool checkUpperBreak(dActor_c *actor);
    bool checkSideBreak(dActor_c *actor, int direction);

    void breakEffect();
    void meltEffect();
    void yoganEffect();
    void poisonEffect();

    void breakReq(int dir, int playerNo);
    void floatbreakReq();

    bool checkNetIn();

    void setCarry(dActor_c *actor);
    mVec3_c calcCarryPos();
    BOOL canCarry() const;

    bool checkPlHasami(int *);
    bool checkPlAttach_Head(int *);
    bool checkPlAttach_Foot(int *, int *);
    bool checkPlAttach_Wall(int dir, int *);
    bool checkPlHasami_Wall(dActor_c *, int);

    fBaseID_e getParentID();

    STATE_FUNC_DECLARE(daIce_c, Freeze);
    STATE_FUNC_DECLARE(daIce_c, Cover);
    STATE_FUNC_DECLARE(daIce_c, Hop);
    STATE_FUNC_DECLARE(daIce_c, Sink);
    STATE_FUNC_DECLARE(daIce_c, PushSink);
    STATE_FUNC_DECLARE(daIce_c, Float);
    STATE_FUNC_DECLARE(daIce_c, SmallFloat);
    STATE_FUNC_DECLARE(daIce_c, FloatWait);
    STATE_FUNC_DECLARE(daIce_c, TogeFloat);
    STATE_FUNC_DECLARE(daIce_c, ShakeFloat);
    STATE_FUNC_DECLARE(daIce_c, Air);
    STATE_FUNC_DECLARE(daIce_c, Fall);
    STATE_FUNC_DECLARE(daIce_c, Land);
    STATE_FUNC_DECLARE(daIce_c, Dive);
    STATE_FUNC_DECLARE(daIce_c, YoganDive);
    STATE_FUNC_DECLARE(daIce_c, Carry);
    STATE_FUNC_DECLARE(daIce_c, Throw);
    STATE_FUNC_DECLARE(daIce_c, Drop);
    STATE_FUNC_DECLARE(daIce_c, Melt_Normal);

    bool isDestroy() { return m_60c != 0 || mpIceMng != nullptr && mpIceMng->mDestroyMode != dIceMng_c::DESTROY_NONE; }

    static void callBackFoot(dActor_c *self, dActor_c *other);
    static void callBackHead(dActor_c *self, dActor_c *other);
    static void callBackWall(dActor_c *self, dActor_c *other, u8 dir);
    static bool checkRevFoot(dActor_c *self, dActor_c *other);
    static bool checkRevHead(dActor_c *self, dActor_c *other);
    static bool checkRevWall(dActor_c *self, dActor_c *other, u8 dir);
    static void ccCallBack(dCc_c *self, dCc_c *other);
    static void ccCallBack_Water(dCc_c *self, dCc_c *other);

private:
    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mResFileMdl;
    m3d::mdl_c mMdl;
    nw4r::g3d::ResAnmTexSrt mResFileAnm;
    m3d::anmTexSrt_c mAnm;
    dBg_ctr_c mBgCtr;
    float m_544;
    float m_548;
    float m_54c;
    float m_550;
    sBcPointData mSensorFoot;
    sBcPointData mSensorHead;
    sBcPointData mSensorWall;
    mVec3_c mPosDiff;
    mVec3_c mInitialPos;
    mVec3_c m_59c;
    mVec3_c m_5a8;
    mVec3_c mCcSize;
    int m_5c0;
    int mCarriedByPlrNo;
    float mFloatTargetSpeed;
    float mFloatAccel;
    short mFloatAngle;
    u8 mPad1[0x4];
    float m_5d8;
    int m_5dc;
    short mShakeAngle1;
    short mShakeAngle2;
    short mShakeAngle3;
    u8 mPad2[0x4];
    int m_5ec;
    u8 mPad3[0x10];
    int m_600;
    u8 mPad4[0x4];
    u32 m_608;
    int m_60c;
    float m_610;
    IceType_e m_614;
    int mWeightType;
    dIceMng_c *mpIceMng;
    dIceEfMaker_c mIceEfMaker;

    ACTOR_PARAM_CONFIG(IceType, 0, 4);
    ACTOR_PARAM_CONFIG(Param8, 8, 1);
    ACTOR_PARAM_CONFIG(ParamC, 12, 1);
    ACTOR_PARAM_CONFIG(Param10, 16, 2);
    ACTOR_PARAM_CONFIG(Param14, 20, 1);
    ACTOR_PARAM_CONFIG(Param18, 24, 4);

    static const short smc_OBJBG_DUMMY_ANGLE = 0;
};
