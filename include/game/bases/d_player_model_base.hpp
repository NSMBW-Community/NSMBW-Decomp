#pragma once

#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_vec.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/bases/d_heap_allocator.hpp>

class daPlBase_c;

///< @unofficial
enum AnmID_e {
    PLAYER_ANIM_NONE = -1,
    PLAYER_ANIM_WAIT,
    PLAYER_ANIM_WALK,
    PLAYER_ANIM_RUN,
    PLAYER_ANIM_B_DASH,
    PLAYER_ANIM_B_DASH2,
    PLAYER_ANIM_JUMP,
    PLAYER_ANIM_JUMP2,
    PLAYER_ANIM_JUMPED,
    PLAYER_ANIM_2JMP_C_1,
    PLAYER_ANIM_2JMP_C_2,
    PLAYER_ANIM_2JUMPED,
    PLAYER_ANIM_ROLL_JUMP,
    PLAYER_ANIM_2JUMP2,
    PLAYER_ANIM_MAME_JUMP2,
    PLAYER_ANIM_TURN,
    PLAYER_ANIM_TURNED,
    PLAYER_ANIM_HIPSR,
    PLAYER_ANIM_HIPAT,
    PLAYER_ANIM_HIPED,
    PLAYER_ANIM_HIP_TO_STOOP,
    PLAYER_ANIM_STOOP,
    PLAYER_ANIM_STOOP_START,
    PLAYER_ANIM_SLIP,
    PLAYER_ANIM_SLIPED,
    PLAYER_ANIM_SLIP_TO_STOOP,
    PLAYER_ANIM_CARRY_WAIT,
    PLAYER_ANIM_CARRY_WALK,
    PLAYER_ANIM_CARRY_THROW,
    PLAYER_ANIM_RCARRY_WAIT,
    PLAYER_ANIM_WSLD,
    PLAYER_ANIM_FIRE_AT,
    PLAYER_ANIM_SWIM_FIRE_AT,
    PLAYER_ANIM_SWIM_FIRE_AT2,
    PLAYER_ANIM_STAR_ROLL,
    PLAYER_ANIM_P_SWIM,
    PLAYER_ANIM_SWIM,
    PLAYER_ANIM_SWIM_WAIT,
    PLAYER_ANIM_SWIM_THROW,
    PLAYER_ANIM_SWIM_WALK,
    PLAYER_ANIM_SWIM_STANDING,
    PLAYER_ANIM_PADDLE_1,
    PLAYER_ANIM_PADDLE_2,
    PLAYER_ANIM_PADDLE_CARRY,
    PLAYER_ANIM_TREE_START,
    PLAYER_ANIM_TREE_WAIT,
    PLAYER_ANIM_TREE_CLIMB,
    PLAYER_ANIM_TREE_POSE,
    PLAYER_ANIM_MONKEY_START,
    PLAYER_ANIM_MONKEY_WAIT_R,
    PLAYER_ANIM_MONKEY_WAIT_L,
    PLAYER_ANIM_MONKEY_R_TO_L,
    PLAYER_ANIM_MONKEY_L_TO_R,
    PLAYER_ANIM_NET_WAIT,
    PLAYER_ANIM_NET_WALK1,
    PLAYER_ANIM_NET_WALK2,
    PLAYER_ANIM_NET_ATTACK,
    PLAYER_ANIM_PEA_PLANT,
    PLAYER_ANIM_PEA_PLANT_ST,
    PLAYER_ANIM_PEA_PLANT_WAIT,
    PLAYER_ANIM_WALL_WAIT,
    PLAYER_ANIM_WALL_WALK_L,
    PLAYER_ANIM_WALL_WALK_R,
    PLAYER_ANIM_HANG_START,
    PLAYER_ANIM_HANG_UP,
    PLAYER_ANIM_HANG_WAIT,
    PLAYER_ANIM_HANG_WALK_L,
    PLAYER_ANIM_HANG_WALK_R,
    PLAYER_ANIM_W_JUMP1,
    PLAYER_ANIM_W_JUMP2,
    PLAYER_ANIM_2JUMPED_DUPLICATE,
    PLAYER_ANIM_JUMP_HANG,
    PLAYER_ANIM_SPIN_ST,
    PLAYER_ANIM_SPIN_END,
    PLAYER_ANIM_SPIN_LOW_ST,
    PLAYER_ANIM_SPIN_LOW_ED,
    PLAYER_ANIM_SPIN_JUMP2,
    PLAYER_ANIM_SPIN_JUMP_END,
    PLAYER_ANIM_DAM_F,
    PLAYER_ANIM_DAM_B,
    PLAYER_ANIM_DOW_F,
    PLAYER_ANIM_DOW_B,
    PLAYER_ANIM_FIREJMP,
    PLAYER_ANIM_E_SHOCK,
    PLAYER_ANIM_DEAD,
    PLAYER_ANIM_DEAD_POSE,
    PLAYER_ANIM_RTREE_START,
    PLAYER_ANIM_RTREE_WAIT,
    PLAYER_ANIM_RTREE_POSE,
    PLAYER_ANIM_GOAL_JUMP,
    PLAYER_ANIM_GOAL_JUMP_ED,
    PLAYER_ANIM_GOAL_PUTON_CAP,
    PLAYER_ANIM_PL_GOAL_PUTON_CAP,
    PLAYER_ANIM_P_GOAL_PUTON_CAP,
    PLAYER_ANIM_PL_RGOAL_PUTON_CAP,
    PLAYER_ANIM_P_RGOAL_PUTON_CAP,
    PLAYER_ANIM_GOAL_PUTON_CAPF,
    PLAYER_ANIM_R_EAT,
    PLAYER_ANIM_R_EAT_OUT,
    PLAYER_ANIM_R_EAT_SUCCESS,
    PLAYER_ANIM_R_EAT_FAIL,
    PLAYER_ANIM_R_EAT_SUCCESSB,
    PLAYER_ANIM_R_EAT_SUCCESSB_DUPLICATE,
    PLAYER_ANIM_RS_EAT,
    PLAYER_ANIM_RS_EAT_OUT,
    PLAYER_ANIM_RS_EAT_SUCCESS,
    PLAYER_ANIM_RS_EAT_FAIL,
    PLAYER_ANIM_RS_EAT_SUCCESSB,
    PLAYER_ANIM_RS_EAT_SUCCESS_DUPLICATE,
    PLAYER_ANIM_CARRY_P_START,
    PLAYER_ANIM_CARRY_P_WAIT,
    PLAYER_ANIM_CARRY_P_WALK,
    PLAYER_ANIM_CARRY_P_THROW,
    PLAYER_ANIM_CARRY_P_WAIT_DUPLICATE,
    PLAYER_ANIM_RIDE_ON,
    PLAYER_ANIM_F_JUMP,
    PLAYER_ANIM_SET,
    PLAYER_ANIM_SET_TO_WAIT,
    PLAYER_ANIM_STAR_ROLL_DUPLICATE,
    PLAYER_ANIM_S_JUMP,
    PLAYER_ANIM_S_JUMP2,
    PLAYER_ANIM_S_JUMPED,
    PLAYER_ANIM_GET_DOWN,
    PLAYER_ANIM_ICE_TURN,
    PLAYER_ANIM_ICE_TURNED,
    PLAYER_ANIM_ICE_SLIP_F,
    PLAYER_ANIM_ICE_SLIP_B,
    PLAYER_ANIM_ROPE_SWING,
    PLAYER_ANIM_SHOOT,
    PLAYER_ANIM_SHOOT_SLIP,
    PLAYER_ANIM_SHOOT_SLIP_END,
    PLAYER_ANIM_LOW_WALK_START,
    PLAYER_ANIM_LOW_WALK,
    PLAYER_ANIM_SWIM_PIPE,
    PLAYER_ANIM_DOOR_WALK,
    PLAYER_ANIM_PL_SPIN_JUMP,
    PLAYER_ANIM_WAIT_L,
    PLAYER_ANIM_WAIT_R,
    PLAYER_ANIM_FIRE_AT2,
    PLAYER_ANIM_BLOW_UP,
    PLAYER_ANIM_P_SLIP,
    PLAYER_ANIM_P_SLIP_JUMP,
    PLAYER_ANIM_P_SLIP_JUMP2,
    PLAYER_ANIM_RF_JUMP,
    PLAYER_ANIM_STAMP,
    PLAYER_ANIM_WAIT_R_DUPLICATE,
    PLAYER_ANIM_WAIT_L_DUPLICATE,
    PLAYER_ANIM_WAIT_R3,
    PLAYER_ANIM_WAIT_L3,
    PLAYER_ANIM_BOSS_KEY_GET,
    PLAYER_ANIM_BALLOON_WAIT,
    PLAYER_ANIM_SLOPE_WAIT_L,
    PLAYER_ANIM_SLOPE_WAIT_R,
    PLAYER_ANIM_SLOPE_WAIT_L2,
    PLAYER_ANIM_SLOPE_WAIT_R2,
    PLAYER_ANIM_CARRY_P_WAIT_L,
    PLAYER_ANIM_CARRY_P_WAIT_R,
    PLAYER_ANIM_CARRY_WAIT_L,
    PLAYER_ANIM_CARRY_WAIT_R,
    PLAYER_ANIM_SPIN_JUMP3,
    PLAYER_ANIM_RIDE_WAIT,
    PLAYER_ANIM_P_PADDLE_1,
    PLAYER_ANIM_P_PADDLE_2,
    PLAYER_ANIM_POSE_L,
    PLAYER_ANIM_POSE_R,
    PLAYER_ANIM_GORL_WAIT,
    PLAYER_ANIM_DM_NOTICE,
    PLAYER_ANIM_DM_NOTI_WAIT,
    PLAYER_ANIM_DM_SURPRISE,
    PLAYER_ANIM_DM_SURP_WAIT,
    PLAYER_ANIM_WAIT_SELECT,
    PLAYER_ANIM_COURSE_IN,
    PLAYER_ANIM_WAIT_DUPLICATE,
    PLAYER_ANIM_WAIT_DUPLICATE2,
    PLAYER_ANIM_DM_ESCORT,
    PLAYER_ANIM_DM_GLAD,
    PLAYER_ANIM_ENDING_WAIT,
    PLAYER_ANIM_COIN_COMP,
    PLAYER_ANIM_COUNT
};

class dPyMdlBase_c {
public:
    /// The randomness distribution according to which the jump animation is.
    /// Chosen in dPyMdlBase_c::setJumpAnmRand.
    enum RndType_e {
        RND_EQUAL, ///< 33% 0, 33% 1, 33% 2
        RND_WEIGHTED ///< 60% 0, 30% 1, 10% 2
    };

    enum TexAnmType_e {
        TEX_ANM_0,
        TEX_ANM_1,
        TEX_ANM_2,
        TEX_ANM_3,
        TEX_ANM_4,
        TEX_ANM_5,
        TEX_ANM_6,
        TEX_ANM_7,
        TEX_ANM_8,
        TEX_ANM_9,
        TEX_ANM_A,
        TEX_ANM_B,
        TEX_ANM_C,
        TEX_ANM_D
    };

    struct AnmData_s {
        const char *mName; ///< Regular animation name.
        const char *mRideName; ///< Animation name when riding.
        const char *mYoshiName; ///< Yoshi personal animation name.
        const char *mPlayerName; ///< Player 3 personal animation name.
        m3d::playMode_e mPlayMode;
        float mRate;
        float mBlendDuration;
        TexAnmType_e mTexAnmType;
        u32 mFlags; ///< Same type as dPyMdlBase_c's mFlags.
    };

    dPyMdlBase_c(u8);
    virtual ~dPyMdlBase_c();
    virtual int getFaceJointIdx() const { return 0; }
    virtual void createModel();
    virtual void initialize();
    virtual void play();
    virtual void _calc();
    virtual void calc2() {}
    virtual void draw();
    virtual m3d::bmdl_c *getBodyMdl() { return nullptr; }
    virtual const nw4r::g3d::ResFile *getAnmResFile() const;
    virtual void setPlayerMode(int mode);
    virtual void setColorType(u8 type);
    virtual void setDark(int) {}
    virtual void setFunbariRate(float) {}
    virtual void onStarAnm() { m_17c |= 0x100; }
    virtual void offStarAnm() { m_17c &= ~0x100; }
    virtual void onStarEffect() { m_17c |= 0x200; };
    virtual void offStarEffect() { m_17c &= ~0x200; };
    virtual void getJointMtx(mMtx_c* mtx, int i);
    virtual bool getHeadPropelJointMtx(mMtx_c *mtx) { return false; }
    virtual bool getJumpAnmName(int jumpType, char *anmNameBuf, int p4); ///< @unofficial
    virtual void setAnm(int animID, float rate, float c, float frame);
    virtual int setPersonalAnm(int, nw4r::g3d::ResAnmChr *, int);
    virtual void setBodyAnm(int animID, float rate, float frame, float c);
    virtual void releaseBodyAnm(float);
    virtual void copyAnm();
    virtual void _setFootAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float);
    virtual void _setBodyAnm(nw4r::g3d::ResAnmChr &, m3d::playMode_e, float, float, float);
    virtual void setAnmBind();
    virtual int setPersonalRideAnm(int, nw4r::g3d::ResAnmChr *);
    virtual void setTexAnmType(TexAnmType_e anmType);
    virtual void setFrame(float frame);
    virtual void setBodyFrame(float frame);
    virtual void setRate(float rate);
    virtual void setBodyRate(float rate);
    virtual void setPropelRollSpeed(s16 speed) {}
    virtual s16 getPropelRollSpeed() const { return 0; }
    virtual void setPropelRollAngle(s16 speed) {}
    virtual s16 getPropelRollAngle() const { return 0; }
    virtual void setPropelScale(float scale) {}
    virtual float *getLegLengthP(u8) { return nullptr; }
    virtual void updateBonusCap() {};

    int create(u8, u8 playerMode, int);
    void calc(mMtx_c &baseMtx);
    void calc(mVec3_c pos, mAng3_c rot, mVec3_c scale);
    void setBaseMtx(mVec3_c &pos, mAng3_c &rot, mVec3_c &scale);
    void setSoftLight(m3d::bmdl_c &mdl);
    bool isBodyAnmOn();
    void setJumpAnmRand(RndType_e rndType);
    void setSlopeBodyAnm(float);
    void setCarryBodyAnm(float);
    void setLinkAnm(int, float, float, float);
    void setRideAnm(int anmID, float rate, float b, float frame);

    void calcStoopOffset();

    bool isFootStepTiming();
    void getJointPos(mVec3_c *pos, int i);
    void copyLinkAnm(float f);

    mMtx_c &getMtx() { return mMtx; }

    void setAng(mAng3_c ang) {
        mAngX = ang.x;
        mAngY = ang.y;
        mAngZ = ang.z;
    }

    u32 get151CheckVal() { return 2; }

    m3d::anmChr_c &getFootAnm() { return mAnms[0]; }
    m3d::anmChr_c &getBodyAnm() { return mAnms[1]; }

    dHeapAllocator_c mAllocator;
    daPlBase_c *mpOwner;
    dPyMdlBase_c *mpSpinLiftParentMdl;
    m3d::anmChr_c mAnms[2];
    m3d::anmChrBlend_c mBlendAnm;
    mVec3_c mHeadPosMaybe;
    mVec3_c mHatPosMaybe;
    mMtx_c mBaseMtx;
    mMtx_c mMtx;
    mVec3_c mHeadOffset;
    mVec3_c mScale;
    u8 mAnotherPlayerID;
    u8 m_151;
    u8 mPlayerMode;
    u8 mPowerupTex;
    int mCurrAnmID; ///< Is an AnmID_e.
    int mPrevAnmID; ///< Is an AnmID_e.
    int m_15c; ///< Is an AnmID_e.
    u32 mFlags;
    u32 m_164;
    u32 mJumpAnmVariant;
    u32 mPrevJumpAnmVariant;
    int mCurrHeadPatID;
    int mNextPatSwitchTimer;
    u8 m_178;
    /* 3 padding bytes */
    u32 m_17c;
    int m_180;
    float mStoopOffset;
    float mStoopOffsetTarget;
    int mStoopTimer;
    mVec3_c m_190_vecs[6];
    u8 mPad9[0x24];
    // [Can't get these to be an mAng3_c because of daPlBase_c::calcHeadAttentionAngle]
    s16 mAngX;
    s16 mAngY;
    s16 mAngZ;
    u32 m_204;
    u32 m_208;

    static const float scWaterCrouchAnmSpeed;
    static const float scFireShootFrame;
    static const char *scJumpAnmVarDt[3];
    static const char *sc2JumpAnmVarDt[3];
    static const char *sc2JumpedAnmVarDt[3];
    static const AnmData_s scPyAnmData[PLAYER_ANIM_COUNT];
};
