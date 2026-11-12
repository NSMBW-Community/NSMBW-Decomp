#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_ice_effect_maker.hpp>
#include <game/bases/d_ice_manager.hpp>
#include <game/bases/d_ice_param.hpp>

/// @brief Ice blocks for enemies.
class daIce_c : public dActorState_c {
public:
    /// @unofficial
    enum InitType_e {
        INIT_LAND,
        INIT_FREEZE_1,
        INIT_AIR,
        INIT_FALL,
        INIT_WATER,
        INIT_WATER_TOGE,
        INIT_FREEZE_2,
        INIT_COVER,
        INIT_HOP
    };

    /// @unofficial
    enum IceWeight_e {
        ICE_WEIGHT_LIGHTEST,
        ICE_WEIGHT_LIGHT,
        ICE_WEIGHT_HEAVY,
        ICE_WEIGHT_HEAVIEST,
        ICE_WEIGHT_COUNT
    };

    /// @unofficial
    enum CollisionMode_e {
        COLLISION_NORMAL,
        COLLISION_NONE,
        COLLISION_WATER
    };

    typedef void (daIce_c::*EnlargeFunc)();

    daIce_c() : mBeginShake(0), mFlags(0), mBreakRequested(0), mIceEfMaker(this) {} ///< @copydoc dActorState_c::dActorState_c
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
    void setSpinLiftUpActor(dActor_c *carryingActor) override { setCarry(carryingActor); }
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
    void enlargeBgc(); ///< Enlarges the dynamic collider until it reaches the full size.
    void enlargeBgcLand(); ///< Enlarges the dynamic collider when not in water.
    void enlargeBgcWater(); ///< Enlarges the dynamic collider when in water.

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

    float getSmth() { return mWaterHeight - mSize.y * 0.5f; }
    bool isDestroy() { return mBreakRequested != 0 || mpIceMng != nullptr && mpIceMng->mDestroyMode != dIceMng_c::DESTROY_NONE; }

    static void callBackFoot(dActor_c *self, dActor_c *other);
    static void callBackHead(dActor_c *self, dActor_c *other);
    static void callBackWall(dActor_c *self, dActor_c *other, u8 dir);
    static bool checkRevFoot(dActor_c *self, dActor_c *other);
    static bool checkRevHead(dActor_c *self, dActor_c *other);
    static bool checkRevWall(dActor_c *self, dActor_c *other, u8 dir);
    static void ccCallBack(dCc_c *self, dCc_c *other);
    static void ccCallBack_Water(dCc_c *self, dCc_c *other);

private:
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFileMdl; ///< The resource file.
    m3d::mdl_c mMdl; ///< The model.
    nw4r::g3d::ResAnmTexSrt mResFileAnm; ///< The texture animation resource file.
    m3d::anmTexSrt_c mAnm; ///< The texture animation.
    dBg_ctr_c mBgCtr; ///< The dynamic collider.
    mVec2_c mCurrBgcSize; ///< The current dynamic collider size, grows over time.
    mVec2_c mBgcSizeIncrease; ///< The amount to increate #mCurrBgcSize per frame until the full size is reached.
    sBcMany mBcPointFoot;
    sBcMany mBcPointHead;
    sBcMany mBcPointWall;
    mVec3_c mPosDiff; ///< The offset to the parent actor.
    mVec3_c mInitialPos;
    mVec3_c mModelPos;
    mVec3_c mPrevPos;
    mVec3_c mSize;
    int mFloatType;
    int mCarriedByPlrNo;
    float mFloatTargetSpeed;
    float mFloatAccel;
    short mFloatAngle;
    u8 mPad1[0x4];
    float mExtraFloatSpeedX; ///< Added to the X speed while floating. [Seems to never get set].
    BOOL mBeginShake;
    short mShakeAngle1;
    short mShakeAngle2;
    short mShakeAngle3;
    u8 mPad2[0x4];
    int mIsWater;
    short mCurrFloatUpDown; ///< Counter for bobbing up and down on the water surface.
    float mWaterHeight;
    int mFloatState;
    float mStartFallY;
    int mCollEntryTimer;
    int mFallTimer;
    u32 mFlags;
    BOOL mBreakRequested;
    float prevX; ///< The X position before running execute().
    InitType_e mInitType;
    IceWeight_e mWeightType;
    dIceMng_c *mpIceMng;
    dIceEfMaker_c mIceEfMaker;

    ACTOR_PARAM_CONFIG(IceType, 0, 4);
    ACTOR_PARAM_CONFIG(Param8, 8, 1); // Sets mFlags bit 0
    ACTOR_PARAM_CONFIG(ParamC, 12, 1); // Sets mFlags bit 1
    ACTOR_PARAM_CONFIG(Param10, 16, 2);
    ACTOR_PARAM_CONFIG(Param14, 20, 1);
    ACTOR_PARAM_CONFIG(InitType, 24, 4);

    static const short smc_OBJBG_DUMMY_ANGLE = 0;
};
