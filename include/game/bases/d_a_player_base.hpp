#pragma once

#include <constants/game_constants.h>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_ac_py_key.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_effect.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_quake.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/sLib/s_State.hpp>
#include <game/bases/d_quake.hpp>
#include <constants/game_constants.h>

class dPyMdlBase_c {
public:
    virtual ~dPyMdlBase_c();
    virtual void getFaceJointIdx();
    virtual void createModel();
    virtual void initialize();
    virtual void play();
    virtual void _calc();
    virtual void calc2();
    virtual void draw();
    virtual void getBodyMdl();
    virtual void getAnmResFile();
    virtual void setPlayerMode();
    virtual void setColorType();
    virtual void setDark();
    virtual void vf3c(); ///< @unofficial
    virtual void onStarAnm();
    virtual void offStarAnm();
    virtual void onStarEffect();
    virtual void offStarEffect();
    virtual void getJointMtx();
    virtual void getHeadPropelJointMtx();
    virtual void vf58(); ///< @unofficial
    virtual void setAnm(int, float, float, float);
    virtual void vf60(); ///< @unofficial
    virtual void vf64(); ///< @unofficial
    virtual void vf68(); ///< @unofficial
    virtual void copyAnm();
    virtual void vf70(); ///< @unofficial
    virtual void vf74(); ///< @unofficial
    virtual void setAnmBind();
    virtual void vf7c(); ///< @unofficial
    virtual void setTexAnmType();
    virtual void setFrame(float);
    virtual void setBodyFrame();
    virtual void setRate(float);
    virtual void setBodyRate();
    virtual void vf94(); ///< @unofficial
    virtual void getPropelRollSpeed();
    virtual void vf9c(); ///< @unofficial
    virtual void vfa0(); ///< @unofficial
    virtual void vfa4(); ///< @unofficial
    virtual void vfa8(); ///< @unofficial
    virtual void vfac(); ///< @unofficial

    bool isFootStepTiming();
    void getJointPos(mVec3_c *, int);

    float getFrameMax() { return mAnm.mFrameMax; }
    mMtx_c &getMtx() { return mMtx; }

    void setAng(mAng3_c ang) {
        m_1fc = ang.x;
        m_1fe = ang.y;
        m_200 = ang.z;
    }

    u8 mPad1[0x24];
    m3d::anmChr_c mAnm;
    u8 mPad2[0x6c];
    mVec3_c mHatPosMaybe;
    u8 mPad3[0x30];
    mMtx_c mMtx;
    u8 mPad4[0x1a];
    u8 m_152;
    int m_154;
    u8 mPad5[8];
    u32 mFlags;
    u8 mPad6[0x98];
    s16 m_1fc;
    s16 m_1fe;
    s16 m_200;
    u32 m_204;
    u32 m_208;

    static const float scWaterCrouchAnmSpeed;
};

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

class dPyModel_HIO_c {
public:
    float mData[17];
};

class dYoshiModel_HIO_c {
public:
    float mData[4];
};

class dPyMdlBase_HIO_c {
public:
    u8 mPad[0x8];
    float m_08[8];
    dPyAnmMain_HIO_c mPyAnm;
    dPyModel_HIO_c mPyModel[3];
    dYoshiModel_HIO_c mYoshiModel;

    float pyRate(int anmID) const {
        return mPyAnm.mAnm[anmID].mRate;
    }

    float pyBlend(int anmID) const {
        return mPyAnm.mAnm[anmID].mBlendDuration;
    }
};

class dPyMdlMng_c {
public:
    u8 mPad[4];
    dPyMdlBase_c *mpMdl;

    void setAnm(int anmID, float rate, float blendDuration, float f) {
        mpMdl->setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, float blendDuration, float f) {
        float rate = m_hio.mPyAnm.mAnm[anmID].mRate;
        setAnm(anmID, rate, blendDuration, f);
    }

    void setAnm(int anmID, const dPyAnm_HIO_c &hio, float f = 0.0f) {
        mpMdl->setAnm(anmID, hio.mRate, hio.mBlendDuration, f);
    }

    void setAnm(int anmID, float f = 0.0f) {
        setAnm(anmID, m_hio.mPyAnm.mAnm[anmID], f);
    }

    int getAnm() const {
        return mpMdl->m_154;
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

    mVec3_c &getHatPos() const { return mpMdl->mHatPosMaybe; }

    static dPyMdlBase_HIO_c m_hio;
};

class daPlBase_c : public dActor_c {
public:
    enum DamageType_e {
        DAMAGE_NONE = 0,
        DAMAGE_1, DAMAGE_2, DAMAGE_3, DAMAGE_4,
        DAMAGE_5, DAMAGE_6, DAMAGE_7, DAMAGE_8,
        DAMAGE_9, DAMAGE_A, DAMAGE_B, DAMAGE_C
    };

    enum DokanDir_e {
        DOKAN_U,
        DOKAN_D,
        DOKAN_L,
        DOKAN_R,
        DOKAN_ROLL
    };

    enum StarSet_e {};

    enum AnmBlend_e {
        BLEND_0,
        BLEND_1
    };

    /// @unofficial
    enum GroundType_e {
        GROUND_TYPE_DEFAULT,
        GROUND_TYPE_SNOW,
        GROUND_TYPE_SAND,
        GROUND_TYPE_ICE,
        GROUND_TYPE_DIRT,
        GROUND_TYPE_WATER,
        GROUND_TYPE_CLOUD,
        GROUND_TYPE_FUNSUI,
        GROUND_TYPE_MANTA,
        GROUND_TYPE_BEACH,
        GROUND_TYPE_CARPET,
        GROUND_TYPE_LEAF,
        GROUND_TYPE_WOOD
    };

    bool groundTypeCheck1() {
        return (mGroundType < GROUND_TYPE_DEFAULT) ? true : false;
    }

    bool groundTypeCheck2() {
        return (mGroundType > GROUND_TYPE_CLOUD) ? true : false;
    }

    bool groundTypeCheck3() {
        return (mGroundType < GROUND_TYPE_MANTA) ? true : false;
    }

    bool groundTypeCheck4() {
        return (mGroundType > GROUND_TYPE_CARPET) ? true : false;
    }

    bool groundTypeCheck5() {
        return (mGroundType >= GROUND_TYPE_SNOW) ? true : false;
    }

    bool groundTypeCheck6() {
        return (mGroundType <= GROUND_TYPE_ICE) ? true : false;
    }

    bool groundTypeCheck7() {
        return (mGroundType == GROUND_TYPE_WATER) ? true : false;
    }

    bool groundTypeCheck8() {
        return (mGroundType == GROUND_TYPE_FUNSUI) ? true : false;
    }

    bool groundTypeCheck9() {
        return (mGroundType == GROUND_TYPE_BEACH) ? true : false;
    }

    /// @unofficial
    enum SlipSubstate_e {
        SLIP_ACTION_NONE,
        SLIP_ACTION_STOOP,
        SLIP_ACTION_END
    };

    /// @unofficial
    enum HipSubstate_e {
        HIP_ACTION_READY,
        HIP_ACTION_ATTACK_START,
        HIP_ACTION_ATTACK_FALL,
        HIP_ACTION_GROUND,
        HIP_ACTION_STAND_NORMAL,
        HIP_ACTION_STAND_NORMAL_END,
        HIP_ACTION_TO_STOOP
    };

    /// @unofficial
    enum JumpDaiSubstate_e {
        JUMP_DAI_ACTION_0,
        JUMP_DAI_ACTION_1,
    };

    /// @unofficial
    enum FunsuiSubstate_e {
        FUNSUI_ACTION_NONE,
        FUNSUI_ACTION_START
    };

    /// @unofficial
    enum AnimePlaySubstate_e {
        ANIME_PLAY_ACTION_0,
        ANIME_PLAY_ACTION_1,
        ANIME_PLAY_ACTION_2
    };

    /// @unofficial
    enum DemoInDokanSubstate_e {
        DEMO_IN_DOKAN_ACTION_0,
        DEMO_IN_DOKAN_ACTION_1,
        DEMO_IN_DOKAN_ACTION_2,
        DEMO_IN_DOKAN_ACTION_3,
        DEMO_IN_DOKAN_ACTION_4
    };

    /// @unofficial
    enum PowerChangeType_e {
        POWER_CHANGE_0 = 0,
        POWER_CHANGE_1 = 1,
        POWER_CHANGE_2 = 2
    };

    enum Status_e {
        STATUS_01 = 0x01,
        STATUS_02 = 0x02,
        STATUS_03 = 0x03,
        STATUS_04 = 0x04,
        STATUS_06 = 0x06,
        STATUS_08 = 0x08,
        STATUS_0A = 0x0a,
        STATUS_0E = 0x0e,
        STATUS_12 = 0x12,
        STATUS_13 = 0x13,
        STATUS_14 = 0x14,
        STATUS_15 = 0x15,
        STATUS_16 = 0x16,
        STATUS_17 = 0x17,
        STATUS_1A = 0x1a,
        STATUS_1B = 0x1b,
        STATUS_1C = 0x1c,
        STATUS_1D = 0x1d,
        STATUS_1E = 0x1e,
        STATUS_1F = 0x1f,
        STATUS_22 = 0x22,
        STATUS_23 = 0x23,
        STATUS_24 = 0x24,
        STATUS_2A = 0x2a,
        STATUS_2B = 0x2b,
        STATUS_2C = 0x2c,
        STATUS_2D = 0x2d,
        STATUS_2E = 0x2e,
        STATUS_30 = 0x30,
        STATUS_31 = 0x31,
        STATUS_33 = 0x33,
        STATUS_35 = 0x35,
        STATUS_36 = 0x36,
        STATUS_3A = 0x3a,
        STATUS_3B = 0x3b,
        STATUS_3C = 0x3c,
        STATUS_3E = 0x3e,
        STATUS_40 = 0x40,
        STATUS_41 = 0x41,
        STATUS_42 = 0x42,
        STATUS_43 = 0x43,
        STATUS_45 = 0x45,
        STATUS_4B = 0x4b,
        STATUS_4D = 0x4d,
        STATUS_4E = 0x4e,
        STATUS_51 = 0x51,
        STATUS_52 = 0x52,
        STATUS_53 = 0x53,
        STATUS_58 = 0x58,
        STATUS_59 = 0x59,
        STATUS_5B = 0x5b,
        STATUS_5C = 0x5c,
        STATUS_5D = 0x5d,
        STATUS_5E = 0x5e,
        STATUS_5F = 0x5f,
        STATUS_60 = 0x60,
        STATUS_61 = 0x61,
        STATUS_62 = 0x62,
        STATUS_63 = 0x63,
        STATUS_64 = 0x64,
        STATUS_65 = 0x65,
        STATUS_66 = 0x66,
        STATUS_67 = 0x67,
        STATUS_68 = 0x68,
        STATUS_69 = 0x69,
        STATUS_6A = 0x6a,
        STATUS_6B = 0x6b,
        STATUS_6C = 0x6c,
        STATUS_6D = 0x6d,
        STATUS_6E = 0x6e,
        STATUS_6F = 0x6f,
        STATUS_71 = 0x71,
        STATUS_72 = 0x72,
        STATUS_73 = 0x73,
        STATUS_74 = 0x74,
        STATUS_75 = 0x75,
        STATUS_76 = 0x76,
        STATUS_77 = 0x77,
        STATUS_78 = 0x78,
        STATUS_79 = 0x79,
        STATUS_7A = 0x7a,
        STATUS_7D = 0x7d,
        STATUS_7E = 0x7e,
        STATUS_7F = 0x7f,
        STATUS_80 = 0x80,
        STATUS_81 = 0x81,
        STATUS_82 = 0x82,
        STATUS_83 = 0x83,
        STATUS_84 = 0x84,
        STATUS_86 = 0x86,
        STATUS_87 = 0x87,
        STATUS_88 = 0x88,
        STATUS_89 = 0x89,
        STATUS_8B = 0x8b,
        STATUS_8C = 0x8c,
        STATUS_8D = 0x8d,
        STATUS_91 = 0x91,
        STATUS_94 = 0x94,
        STATUS_95 = 0x95,
        STATUS_96 = 0x96,
        STATUS_97 = 0x97,
        STATUS_98 = 0x98,
        STATUS_99 = 0x99,
        STATUS_9F = 0x9f,
        STATUS_A0 = 0xa0,
        STATUS_A4 = 0xa4,
        STATUS_A5 = 0xa5,
        STATUS_A8 = 0xa8,
        STATUS_AA = 0xaa,
        STATUS_AB = 0xab,
        STATUS_AC = 0xac,
        STATUS_AD = 0xad,
        STATUS_AE = 0xae,
        STATUS_B5 = 0xb5,
        STATUS_B6 = 0xb6,
        STATUS_B7 = 0xb7,
        STATUS_B8 = 0xb8,
        STATUS_B9 = 0xb9,
        STATUS_BA = 0xba,
        STATUS_BB = 0xbb,
        STATUS_BC = 0xbc,
        STATUS_BE = 0xbe,
        STATUS_BF = 0xbf,
        STATUS_C1 = 0xc1,
        STATUS_C4 = 0xc4,
    };

    enum DemoType_e {
        DEMO_0,
        DEMO_1,
        DEMO_2,
        DEMO_3,
        DEMO_4,
        DEMO_5,
        DEMO_6,
        DEMO_7,
        DEMO_8
    };

    /// @unofficial
    struct SpeedData_t {
        float data[9];

        SpeedData_t *operator=(const SpeedData_t &other) {
            for (int i = 0; i < 9; ++i) {
                data[i] = other.data[i];
            }
            return this;
        }
    };

    class jmpInf_c {
    public:
        virtual ~jmpInf_c() {};

        float m_04;
        int m_08;
        int m_0c;
    };

    /// @unofficial
    struct sAirTurnPowerData {
        float mNoButton;
        float mStand;
        float mSlowNoDash;
        float mSlowDash;
        float mMedium;
        float mFast;
        float mTurnAround;
    };

    /// @unofficial
    struct sTurnPowerData {
        float mNormal;
        float mSakaUp;
        float mSakaDown;
        float mAir;
    };

    /// @unofficial
    struct sPowerChangeData {
        sAirTurnPowerData mAirPower[2];
        sTurnPowerData mRangeType0[2];
        sTurnPowerData mRangeType1[2];
        sTurnPowerData mRangeType2[2];
    };

    typedef void (daPlBase_c::*ProcFunc)();

    daPlBase_c();
    virtual ~daPlBase_c();

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual int doDelete();
    virtual int preDraw();
    virtual int draw();

    virtual void executeMain() {}
    virtual void executeLastPlayer() {}
    virtual void executeLastAll() {}
    virtual bool isItemKinopio() { return false; }
    virtual void setPowerup(PLAYER_POWERUP_e, int); ///< @unofficial
    virtual u8 getTallType(s8);
    virtual void *getHeadBgPointData() { return nullptr; };
    virtual void *getWallBgPointData() { return nullptr; };
    virtual void *getFootBgPointData() { return nullptr; };
    virtual float getStandHeadBgPointY() { return 0.0f; }
    virtual void checkBgCrossSub() {}
    virtual void postBgCross();
    virtual float getSandSinkRate() { return 0.0f; }
    virtual void setReductionScale();
    virtual void initStampReduction();
    virtual void calcJumpDaiReductionScale(int, int);
    virtual void setReductionBoyon();
    virtual bool setPressBgDamage(int, int);
    virtual bool setBalloonInDispOut(int) { return false; }
    virtual bool isChange() { return false; }
    virtual void changeNextScene(int);
    virtual bool isEnableDokanInStatus(void);
    virtual bool setHideNotGoalPlayer();
    virtual int vf130(float f, mVec2_c *v, int param3);
    virtual bool setDemoCannonWarp(int, short, short) { return false; }

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNone);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoStartWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanRoll);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoRailDokan);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoDown);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNextGotoBlock);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoGoal);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoControl);

    virtual void initialDokanUnder();
    virtual void initialDokanUper();
    virtual void initialDokanRight();
    virtual void initialDokanLeft();
    virtual void initialDokanUnderM();
    virtual void initialDokanUperM();
    virtual void initialDokanRightM();
    virtual void initialDokanLeftM();
    virtual void initialDokanDepth();
    virtual void initialDoor();
    virtual void initialFall();
    virtual void initialVine();
    virtual void initialJumpRight();
    virtual void initialJumpLeft();
    virtual void initialHipAttack();
    virtual void initialSlip();
    virtual void initialSwim();
    virtual void initialBlockJump();
    virtual void initialBlockJumpBelow();
    virtual void initialTorideBoss();
    virtual void initialNormal();

    virtual void setCreateAction(int);
    virtual bool setTimeOverDemo() { return false; }
    virtual void setFallDownDemo() {}
    virtual bool setDokanIn(DokanDir_e dir);
    virtual void initDemoOutDokan();
    virtual bool vf284(int);
    virtual void initDemoGoalBase();
    virtual void executeDemoGoal_Run();
    virtual void initializeDemoControl() {}

    virtual void changeState(const sStateIDIf_c &, void *);

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, None);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Walk);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, SitJump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Fall);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Land);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Crouch);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Slip);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, HipAttack);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Swim);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, JumpDai);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, PlayerJumpDai);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Funsui);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Kani);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Cloud);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, AnimePlay);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, WaitJump);

    virtual bool isWaitFrameCountMax() { return false; }
    virtual bool checkWalkNextAction() { return false; }
    virtual void setWaitActionAnm(AnmBlend_e);
    virtual void setWalkActionAnm(AnmBlend_e);
    virtual void walkActionInit_Wait(AnmBlend_e);
    virtual void walkAction_Wait();
    virtual void walkActionInit_Move(AnmBlend_e);
    virtual void walkAction_Move();
    virtual bool checkCrouch();
    virtual bool setCancelCrouch();
    virtual void setSlipAction();
    virtual void slipActionMove(int);
    virtual void setFallAction();
    virtual void setHipAttack_AttackStart();

    virtual void releaseFunsuiAction();

    virtual float getCloudOffsetY();
    virtual bool setRideJrCrown(const dActor_c *) { return false; }
    virtual bool isRideJrCrownOwn(const dActor_c *) { return false; }
    virtual void setRideJrCrownMtx(const mMtx_c *) {}
    virtual void setRideJrCrownAnm(int) {}

    virtual float *getHeadTopPosP() { return nullptr; }
    virtual float *getGravityData() { return mGravityData; }
    virtual bool isCarry() const { return false; }
    virtual bool isLiftUp() { return false; }

    virtual bool isLiftUpExceptMame() { return false; }
    virtual int isStar() const;
    virtual void setStar(daPlBase_c::StarSet_e, int);
    virtual void endStar() {}
    virtual void setVirusStar(daPlBase_c *) {}
    virtual void clearStarCount();
    virtual s8 getStarCount() const { return mStarCount; }
    virtual s8 calcStarCount(int);

    virtual bool isNoDamage();
    virtual bool setDamage(dActor_c *, DamageType_e);
    virtual bool setForcedDamage(dActor_c *, DamageType_e);

    virtual u32 vf3fc(float, float, int, int, int);
    virtual u32 vf400(float, float, int, int, int);
    virtual bool setWaitJump(float);

    virtual bool setHipAttackOnEnemy(mVec3_c *);

    virtual void clearJumpActionInfo(int) {}
    virtual bool setSwimSpeed(float, float);
    virtual void setLandSE();

    virtual void set1UpKinokoEffect() {}
    virtual void setFlagGetEffect() {}
    virtual void setHipAttackEffect();

    virtual void setZPosition();
    virtual void setZPosition(float);
    virtual void setZPositionDirect(float);
    virtual void offZPosSetNone();

    virtual void vf434(int, int);
    virtual void vf438(int, int);

    virtual void startQuakeShock(dQuake_c::TYPE_SHOCK_e);
    virtual void startPatternRumble(const char *pattern);

    virtual short getMukiAngle(u8 direction);
    virtual int turnAngle();
    virtual void maxFallSpeedSet();

    virtual bool setDamage2(dActor_c *, daPlBase_c::DamageType_e);

    void executeState();
    void changeDemoState(const sStateIDIf_c &, int);

    void onStatus(int);
    void offStatus(int);
    bool isStatus(int);
    void clearFollowMameKuribo();
    void clearCcPlayerRev();
    void clearComboCount();
    void updateEndingDance();
    int addCalcAngleY(short, short);
    short getBesideMukiAngle(u8 direction);
    void turnBesideAngle();
    bool checkRideActor(daPlBase_c *other);
    void setRunFootEffect();
    void calcSpeedOnIceLift();
    void calcAccOnIceLift();
    bool setCrouchJump();
    bool checkStandUpRoofOnLift();
    bool checkStandUpRoof();
    void gravitySet();
    void moveSpeedSet();
    void powerSet();
    bool checkJumpTrigger();
    void changeActionSlipEnd(AnmBlend_e);
    void setSlipAction_ToStoop();
    void setSlipAction_ToEnd();
    void setSlipActionEnd();
    void setSlipActionViewLimitEnd();
    bool checkSakaReverse();
    bool checkBGCrossWall(u8 direction);
    bool checkTurn();
    void setTurnEnd();
    void setVsPlHipAttackEffect();
    void setLandSmokeEffect(int);
    void fn_80057e70(ulong soundID, bool);
    void fn_80057ee0(ulong soundID, short, bool);
    void fn_80057f60(ulong soundID, bool);
    void fn_80057fd0(ulong soundID, short, bool);
    void setHipAttackDropEffect();
    void setHipBlockBreak();
    void setHipAttack_Ready();
    void setHipAttack_KinopioStart();
    void setHipAttack_AttackFall();
    void setHipAttack_StandNormal();
    void setHipAttack_StandNormalEnd();
    void setHipAttack_ToStoop();
    void HipAction_Ready();
    void HipAction_AttackStart();
    void HipAction_AttackFall();
    void HipAction_Ground();
    void HipAction_StandNormal();
    void HipAction_StandNormalEnd();
    void HipAction_ToStoop();
    bool isSlipSaka();
    bool isSaka();
    bool setJumpDaiRide();
    bool setPlayerJumpDai(daPlBase_c *other);
    void setPlayerJumoDaiPos();
    void setNoHitPlayer(const daPlBase_c *, int);
    void updateNoHitPlayer();
    bool isMameAction();
    bool isDemo();
    void setControlDemoWait();
    void initDemoKimePose();
    void fn_80051d00(int);
    void airPowerSet();
    bool checkSlip();
    bool checkCrouchSlip();
    bool checkSlipEndKey();
    float getSlipMaxSpeedF();
    float getSakaMaxSpeedRatio(u8 direction);
    float getSakaStopAccele(u8 direction);
    float getSakaMoveAccele(u8 direction);
    float getIceSakaSlipOffSpeed();
    PowerChangeType_e getPowerChangeType(bool);
    float *getSpeedData();
    void calcWindSpeed();
    void startFootSoundPlayer(unsigned long);
    void setSlipSE();
    bool setSandFunsuiLandEffect();
    void setLandSmokeEffectLight();
    void setStartJumpEffect(int);
    bool setSandJumpEffect();
    void setLandJumpEffect(int);
    void setSlipOnWaterEffect(mEf::levelEffect_c *effect);
    void setSlipSmokeEffect();
    void setBrakeSmokeEffect(mVec3_c &offset);
    void setTurnSmokeEffect();
    void fadeOutTurnEffect();
    void setSandEffect();
    void setSoundPlyMode();
    void setFootSound();
    void bgCheck(int);
    bool startControlDemo();
    bool isDemoType(DemoType_e);
    bool isDemoMode() const;
    bool isControlDemoAll();
    bool isDemoAll();
    void onDemo();
    void offDemo();
    bool isPlayerGameStop();
    void stopOther();
    void playOther();
    void changeNormalAction();
    bool checkTimeOut();
    bool executeDemoState();
    void initDemoInDokanUD(u8);
    void initDemoInDokanLR(u8);
    void endDemoInDokan();
    void executeDemoInDokan(u8);
    float getWaterDokanCenterOffset(float);
    void initDemoInDokan();
    bool demo_dokan_move_x(float, float);
    bool demo_dokan_move_y(float, float);
    bool setDemoOutDokanAction(int, DokanDir_e dir);
    void endDemoOutDokan();
    void initDemoOutDokanUD(u8);
    void executeDemoOutDokanUD();
    void initDemoOutDokanLR(u8);
    void executeDemoOutDokanLR();
    void checkWater();
    void setExitRailDokan();
    void stopGoalOther();
    void playGoalOther();
    void setDemoGoalMode(int, int);
    void finalizeDemoGoalBase();
    float getDemoGoalLandPos();
    void setDemoGoal_MultiJump();
    void initGoalJump(mVec3_c &, float);
    void executeDemoGoal_Pole();
    bool calcGoalJump();
    void executeDemoGoal_Wait();
    void executeDemoGoal_KimePose();
    void setObjDokanIn(dBg_ctr_c *, mVec3_c &, int);
    bool isDispOutCheckOn();
    void endControlDemo(int);
    void setControlDemoDir(u8);
    bool isControlDemoWait();
    void setControlDemoWalk(const float &, const float &);
    bool isControlDemoWalk();
    void setControlDemoAnm(int);
    bool isControlDemoAnm(int);
    void fn_80052290(int);
    void setControlDemoKinopioWalk();
    void setControlDemoKinopioSwim();
    void setControlDemoEndingDance();
    bool isBossDemoLand();
    bool fn_80052500(int, float, int);
    bool isHitWallKinopioWalk(int);
    bool checkKinopioWaitBG(int);
    void fn_80052ef0(int, int, int);
    bool setEnemyStageClearDemo();
    void calcHeadAttentionAngle();
    bool calcSideLimitMultL(float);
    bool calcSideLimitMultR(float);
    bool checkDispSideLemit();
    bool revSideLimitCommon(float);
    void fn_80055d00();
    void underOverCheck();
    void checkDispOver();
    bool checkPressBg();
    void checkDisplayOutDead();
    bool isBgPress(dActor_c *);
    bool checkInsideCrossBg(float);

    int getCcLineKind();
    void initCollision(sCcDatNewF *dat1, sCcDatNewF *dat2);
    void releaseCcData();
    void clearCcData();
    void setCcAtBody(int);
    void setCcAtSlip();
    void setCcAtPenguinSlip();
    void setCcAtHipAttack();
    void setCcAtStar();
    void setCcAtCannon();
    void entryCollision();
    bool isActionRevisionY();
    void setCcPlayerRev(dCc_c *, dCc_c *, float, int);
    bool calcCcPlayerRev(float *);
    bool isEnableStampPlayerJump(dCc_c *, dCc_c *);
    void setStampReduction();
    void setStampPlayerJump(bool b, float f);
    void calcReductionScale();
    mVec3_c getReductionModelScale();
    void checkSideViewLemit();
    bool checkSinkSand();
    void fn_80056370(dActor_c *, int);
    bool isCarryObjBgCarried(u8);
    float getWaterCheckPosY();
    bool setBgDamage();
    bool isEnablePressUD();
    bool isEnablePressLR();
    void setStatus87(); ///< @unofficial
    bool isRideCheckEnable();
    void setStatus5D(float f); ///< @unofficial

    daPlBase_c *getHipAttackDamagePlayer();
    void setHipAttackDamagePlayer(daPlBase_c *player);
    void clearHipAttackDamagePlayer();

    void setNoHitObjBg(dActor_c *, int);
    void calcNoHitObjBgTimer();

    void setOldBGCross();
    void clearBgCheckInfo();
    void checkBgCross();
    void checkDamageBg();

    bool setSandMoveSpeed();

    bool setFunsui();
    bool updateFunsuiPos(float, float);
    bool releaseFunsui(float);

    bool setCloudOn(dActor_c *cloudActor);
    void cancelCloudOn();
    mVec3_c getCloudPos();
    bool updateCloudMove();

    void DemoAnmNormal();
    void DemoAnmBossSetUp();
    void DemoAnmBossGlad();
    void DemoAnmBossAttention();
    void DemoAnmBossKeyGet();

    void simpleMoveSpeedSet();
    void grandPowerSet(); // (misspelling of "ground")
    void slipPowerSet(int);

    void getTurnPower(sTurnPowerData &); ///< @unofficial
    void icePowerChange(int);
    void normalPowerSet();
    void fn_8004bf80(SpeedData_t *data);

    void setJumpGravity();
    void setButtonJumpGravity();
    void setNormalJumpGravity();

    float calcStarAccel(float f) { return 3.0f * f; }
    float calcIdkAccel(float f) { return 0.375f * f; }
    void set_m_d80(int i, float f) { m_d80[i] = f; }
    float getSomeYOffset() const { return mSomeYOffset; }

    // [Needed to place getOldStateID in the correct location]
    void dummy() {
        mStateMgr.getOldStateID();
    }

    bool checkD40Status(int bit) const {
        if (m_d40 & (1 << bit)) {
            return true;
        } else {
            return false;
        }
    }

    float get_1064() const { return m_1064; }
    float get_106c() const { return m_106c; }


    bool checkD44Status(int bit) const {
        if (m_d44 & (1 << bit)) {
            return true;
        } else {
            return false;
        }
    }

    void setD40Status(int bit) {
        m_d40 |= (1 << bit);
    }

    void setD44Status(int bit) {
        m_d44 |= (1 << bit);
    }

    void setStatus(int);
    void calcTimerProc();
    dPyMdlBase_c *getModel();
    void calcPlayerSpeedXY();
    void posMoveAnglePenguin(mVec3_c, u16);
    void posMoveAnglePlayer(mVec3_c);
    float setJumpAddSpeedF(float);
    float setAddLiftSpeedF();
    bool setDelayHelpJump();
    bool fn_800579c0(int, int);
    void onFollowMameKuribo();
    u32 getFollowMameKuribo();
    bool isMaskDraw();
    void setRideNat(float);
    void updateRideNat();
    bool fn_80057E00(int);
    void setItemCompleteVoice();
    void clearTreadCount();
    s8 calcTreadCount(int);
    s8 calcComboCount(int);
    mVec3_c getAnkleCenterPos();

    float getAnkleCenterX() { return getAnkleCenterPos().x; }
    float getAnkleCenterY() { return getAnkleCenterPos().y; }

    int getTreadCount() { return mTreadCount; }

    int m_00;
    int m_04;
    float m_08;
    int mTimer_0c;
    int mTimer_10;
    const daPlBase_c *mpNoHitPlayer;
    int mNoHitTimer;
    u32 m_1c;
    u32 m_20;
    fBaseID_e mIDs[13];
    float m_58;
    int m_5c;
    s8 m_60;
    int mDokanNextGoto;
    mVec3_c m_68;
    mVec2_c m_74;
    short m_7c;
    int m_80;
    u8 m_84;
    dBg_ctr_c *mpBgCtr;
    u8 m_8c;
    u8 m_8d;
    float m_90;
    float m_94;
    short m_98;
    short m_9a;
    int m_9c;
    int m_a0;
    float m_a4;
    int mTimer_a8;
    mVec3_c m_ac;
    int m_b8;
    float m_bc;
    u8 mPad9[0x4];
    int m_c4;
    float m_c8;
    int m_cc;
    int m_d0;
    int mBossDemoLandTimer;
    int m_d8[5];
    int m_ec;
    u8 mPad11[0x4];
    int mTimer_f4;
    int mTimer_f8;
    s8 m_fc;
    dEf::followEffect_c mFollowEf;
    mEf::levelEffect_c mLevelEf1;
    u32 m_344;
    mVec3_c m_348;
    float m_354;
    int mFallTimer;
    int m_35c;
    u32 m_360;
    mEf::levelEffect_c mLevelEfs2;
    mEf::levelEffect_c mLevelEfs3;
    mEf::levelEffect_c mLevelEfs4;
    mEf::levelEffect_c mLevelEfs5;
    mEf::levelEffect_c mLevelEfs6;
    mEf::levelEffect_c mLevelEfs7;
    dPyMdlMng_c *mpMdlMng;
    dAudio::SndObjctPly_c mSndObj;
    dAcPyKey_c mKey;
    fBaseID_e mRideActorID;
    u8 mPad12[0x4];
    fBaseID_e mHipAttackPlayerID;
    u32 mStatusFlags[7];
    float mSomeYOffset;
    u8 mPad14[1];
    u8 m_ca1;
    u8 m_ca2;
    u8 mPad15[1];
    mVec3_c m_ca4;
    mVec3_c m_cb0;
    float m_cbc;
    float m_cc0;
    float m_cc4;
    float m_cc8;
    float *m_ccc;
    float *m_cd0;
    float *mGravityData;
    int mNoGravityTimer;
    u32 m_cdc;
    int mTimer_ce0;
    int mTimer_ce4;
    int mTimer_ce8;
    s8 mTreadCount;
    s8 mStarCount;
    s8 mPlComboCount;
    u32 m_cf0;
    u32 mFollowMameKuribo;
    u8 mPad17_5[4];
    PLAYER_POWERUP_e mPowerup;
    u8 mPad18[0x2c];
    int m_d2c;
    mVec3_c m_d30;
    float m_d3c;
    u32 m_d40;
    u32 m_d44;
    u32 m_d48;
    u32 m_d4c;
    u32 mBgCrossHistory[10];
    u32 m_d78;
    u32 m_d7c;
    float m_d80[2];
    GroundType_e mGroundType;
    float m_d8c;
    int mNoHitObjTimer;
    mAng m_d94;
    short m_d96;
    mAng m_d98, m_d9a, m_d9c;
    int m_da0;
    float m_da4;
    float m_da8;
    u32 m_dac;
    float m_db0;
    u8 m_db4;
    s8 m_db5;
    u8 m_db6;
    mVec3_c m_db8;
    short m_dc4;
    float m_dc8;
    float m_dcc;
    dCc_c mCc1, mAttCc1, mAttCc2, mAttCc3;
    float m_1060;
    float m_1064;
    float m_1068;
    float m_106c;
    bool m_1070;
    bool m_1071;
    int mTimer_1074;
    u8 m_1078;
    u8 m_1079;
    float m_107c;
    float m_1080;
    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mDemoStateMgr;
    void *mDemoStateChangeParam; ///< To be used as a kind of argument to the new demo state.
    int mDemoSubstate; ///< Demo states can use this as a kind of sub-state variable (cast to some enum)
    int m_10c8;
    u8 mDemoMode;
    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mStateMgr;
    void *mStateChangeParam; ///< To be used as a kind of argument to the new state.
    int mSubstate; ///< States can use this as a kind of sub-state variable (cast to some enum)
    int m_1114;
    int m_1118;
    mVec2_c m_111c;
    u8 mPad25[0x4];
    int m_1128;
    float m_112c;
    float m_1130;
    float m_1134;
    float m_1138;
    float m_113c;
    int m_1140;

    static const float sc_DirSpeed[2];
    static const float sc_JumpSpeed;
    static const float sc_JumpSpeedNuma1;
    static const float sc_JumpSpeedNuma2;
    static const float sc_WaterWalkSpeed;
    static const float sc_WaterSwimSpeed;
    static const float sc_WaterJumpSpeed;
    static const float sc_WaterMaxFallSpeed;
    static const float sc_MaxFallSpeed;
    static const float sc_MaxFallSpeed_Foot;
    static const float sc_MaxFallDownSpeed;
    static const float scTurnPowerUpRate;
    static const float scDokanInSpeedX;
    static const float scDokanInWidthX;
    static const float scDokanInMoveSpeed;
    static const float scDokanWaitAnmFixFrame;
};
