#pragma once

#include <game/bases/d_player_model_base.hpp>

class dPyAnm_HIO_c {
public:
    u8 mID;
    float mRate;
    float mBlendDuration;
};

class dPyAnmMain_HIO_c {
public:
    dPyAnm_HIO_c mAnm[177];
};

/// @unofficial
struct dPyModelData_s {
    float a, b, c, d;
};

class dPyModel_HIO_c {
public:
    float mData[5];

    dPyModelData_s getModelData(u8 index) {
        return modelData[index];
    }

    dPyModelData_s modelData[3];
};

class dYoshiModel_HIO_c {
public:
    float mData[4];
};

class dPyMdlBase_HIO_c {
public:
    u8 changeHioType(u8 hioType);
    float getValue(dPyModelData_s model, u8 powerup); ///< @unofficial

    float getValue(int playerType, int idx, int powerup) {
        dPyModelData_s modelData = mPyModel[changeHioType(playerType)].getModelData(idx);
        return getValue(modelData, powerup);
    }

    enum ScaleIndex_e {};

    float get_08(ScaleIndex_e index) {
        return m_08[index];
    }

    u8 mPad[0x8];
    float m_08[8];
    dPyAnmMain_HIO_c mPyAnm;
    dPyModel_HIO_c mPyModel[3];
    dYoshiModel_HIO_c mYoshiModel[3];
};

enum AnmID_e {
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
    PLAYER_ANIM_COIN_COMP
};

class dPyMdlMng_c {
public:
    enum ModelType_e {};
    enum SceneType_e {
        SCENE_TYPE_0
    };

    dPyMdlMng_c(ModelType_e modelType);
    virtual ~dPyMdlMng_c();
    void calc(mMtx_c &);
    void calc(mVec3_c, mAng3_c, mVec3_c);
    void calc2();
    void draw();
    void play();

    void create(u8 playerNo, u8 powerup, SceneType_e sceneType);

    static dPyAnm_HIO_c &getHIO(u8 anmID) {
        return m_hio.mPyAnm.mAnm[anmID];
    }

    void setAnm(int anmID, float rate, float blendDuration, float f) {
        mpMdl->setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, float blendDuration, float f) {
        float rate = getHIO(anmID).mRate;
        setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, const dPyAnm_HIO_c &hio, float f = 0.0f) {
        mpMdl->setAnm(anmID, hio.mRate, hio.mBlendDuration, f);
    }

    void setAnm(int anmID, float f = 0.0f) {
        setAnm(anmID, m_hio.mPyAnm.mAnm[(u8) anmID], f);
    }

    void setAnmOnlyRate(int anmID, const dPyAnm_HIO_c &hio, float a, float b) {
        mpMdl->setAnm(anmID, hio.mRate, a, b);
    }

    void setAnmOnlyRate(int anmID, float a, float b) {
        setAnmOnlyRate(anmID, getHIO(anmID), a, b);
    }

    bool isAnm(int anmID) const {
        return mpMdl->mCurrAnmID == anmID;
    }

    int getAnm() const {
        return mpMdl->mCurrAnmID;
    }

    float getLastFrame() const {
        return mpMdl->mAnm.mFrameMax - 1.0f;
    }

    bool isAnmStop() const {
        return mpMdl->mAnm.isStop();
    }

    mAng3_c getAng() const {
        return mAng3_c(
            mpMdl->m_1fc,
            mpMdl->m_1fe,
            mpMdl->m_200
        );
    }
    void setAng(mAng3_c v) { mpMdl->setAng(v); }

    u32 getFlags() const {
        return mpMdl->mFlags;
    }

    u32 getFlags2() const {
        return mpMdl->m_17c;
    }

    mVec3_c &getHatPos() const { return mpMdl->mHatPosMaybe; }

    float getSomeScale() {
        return m_hio.get_08((dPyMdlBase_HIO_c::ScaleIndex_e) mpMdl->m_152);
    }

    float getSomeScale2() {
        return m_hio.m_08[mpMdl->m_152];
    }

    dPyMdlBase_c *mpMdl;
    u8 mPad[0x4];

    static dPyMdlBase_HIO_c *getHIO() {
        return &m_hio;
    }

    static dPyMdlBase_HIO_c m_hio;
};
