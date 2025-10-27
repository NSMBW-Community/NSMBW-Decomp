#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_en_boyo_manager.hpp>

///< @unofficial
struct CounterCont {
    CounterCont(int clapMode) {
        mCounters1[0] = 0;
        mCounters1[1] = 0;
        mClapMode = clapMode;
    }
    u16 mCounters1[2];
    u16 mCounters2[4];
    u8 mCounter;
    int mClapMode;
};

/// @unofficial
struct block_hit_data_s {
    float x, y;
    float mMaxYSpeed;
    float mYAccel;
    sStateIDIf_c *mDeathState;
    int m14;
    int m18;
    bool mMovingLeft;
    s8 m1d;
    bool mIsInit;
};

struct block_hit {
    block_hit() {
        mData.mIsInit = false;
    }

    void set(block_hit_data_s &other) {
        mData.mIsInit = true;
        float tmpY = other.y;
        float tmpX = other.x;
        mData.x = tmpX;
        mData.y = tmpY;
        mData.mMaxYSpeed = other.mMaxYSpeed;
        mData.mYAccel = other.mYAccel;
        mData.mDeathState = other.mDeathState;
        mData.m14 = other.m14;
        mData.m18 = other.m18;
        mData.mMovingLeft = other.mMovingLeft;
        mData.m1d = other.m1d;
    }

    block_hit_data_s mData;
};

class dIceMng_c {
public:
    dIceMng_c(dActor_c *owner);
    ~dIceMng_c();

    u8 mPad[0x6c];
};

class dEn_c : public dActorMultiState_c {
public:
    dEn_c();
    virtual ~dEn_c();

    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();

    // Overrides in dActor_c

    virtual void block_hit_init();

    virtual void setEatTongue(dActor_c*);
    virtual void setEatTongueOff(dActor_c*);
    virtual bool setEatSpitOut(dActor_c*);

    virtual void calcEatInScale(dActor_c *);

    virtual void yoganSplashEffect(const mVec3_c&, float);
    virtual void poisonSplashEffect(const mVec3_c&, float);

    // Overrides in dActorMultiState_c

    virtual void changeState(const sStateIDIf_c &newState);

    // New virtual functions

    virtual bool EnDamageCheck(dCc_c *cc1, dCc_c *cc2);
    virtual bool PlDamageCheck(dCc_c *cc1, dCc_c *cc2);
    virtual bool YoshiDamageCheck(dCc_c *cc1, dCc_c *cc2);
    virtual bool EtcDamageCheck(dCc_c *cc1, dCc_c *cc2);

    virtual void Normal_VsEnHitCheck(dCc_c *cc1, dCc_c *cc2);
    virtual void Normal_VsPlHitCheck(dCc_c *cc1, dCc_c *cc2);
    virtual void Normal_VsYoshiHitCheck(dCc_c *cc1, dCc_c *cc2);

    virtual bool hitCallback_Star(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Slip(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Large(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Spin(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Rolling(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_WireNet(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_HipAttk(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_YoshiHipAttk(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Screw(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_PenguinSlide(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Cannon(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Shell(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Fire(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Ice(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_YoshiBullet(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_YoshiFire(dCc_c *cc1, dCc_c *cc2);

    virtual void setDeathInfo_Other(dActor_c *actor);
    virtual void setDeathInfo_Quake(int);
    virtual void setDeathInfo_IceBreak();
    virtual void setDeathInfo_IceVanish();

    void setDeathInfo_Smoke(dActor_c *actor);
    //void setDeathInfo_Fumi__5dEn_cFP8dActor_c7mVec2_cRC12sStateIDIf_ci
    //void setDeathInfo_YoshiFumi__5dEn_cFP8dActor_c
    //void setDeathInfo_SpinFumi__5dEn_cFP8dActor_ci

    virtual bool isQuakeDamage();

    virtual void hitYoshiEat(dCc_c *cc1, dCc_c *cc2);

    virtual void setDeathSound_HipAttk();
    virtual void setDeathSound_Fire();
    virtual void setDeathSound_Slip(dActor_c *actor);
    virtual void setDeathSound_Spin();
    virtual void setDeathSound_Rolling();

    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieFumi);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieFall);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieBigFall);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieSmoke);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieYoshiFumi);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieIceVanish);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieGoal);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, DieOther);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, EatIn);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, EatNow);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, EatOut);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, HitSpin);
    STATE_VIRTUAL_FUNC_DECLARE(dEn_c, Ice);

    virtual void fumidamageEffect(const mVec3_c &pos);
    virtual void hipatkEffect(const mVec3_c &pos);
    virtual void fumidamageSE(const mVec3_c &pos, int);

    virtual void quakeAction(); ///< @unofficial

    virtual bool checkDispIn();

    virtual void setWaterSpeed();

    virtual void setDamage(dActor_c *actor);

    virtual void boyonBegin();
    virtual void calcBoyonScale();

    virtual void createIceActor();
    virtual void setIceAnm();
    virtual void returnAnm_Ice();
    virtual void returnState_Ice();

    virtual void isFunsui() const;
    virtual void endFunsui();
    virtual void beginFunsui();

    virtual bool checkComboClap(int max);

    virtual void FumiJumpSet(dActor_c *actor);
    virtual void FumiScoreSet(dActor_c *actor);
    virtual void MameFumiJumpSet(dActor_c *actor);
    virtual void YoshiFumiJumpSet(dActor_c *actor);
    virtual void YoshiFumiScoreSet(dActor_c *actor);
    void SpinFumiJumpSet(dActor_c *actor);
    void SpinFumiScoreSet(dActor_c *actor);
    virtual void fumiSE(dActor_c *actor);
    virtual void fumiEffect(dActor_c *actor);
    virtual void spinfumiSE(dActor_c *actor);
    virtual void spinfumiEffect(dActor_c *actor);
    virtual void mamefumiSE(dActor_c *actor);
    virtual void mamefumiEffect(dActor_c *actor);
    virtual void yoshifumiSE(dActor_c *actor);
    virtual void yoshifumiEffect(dActor_c *actor);
    void fumistepSE(dActor_c *actor);
    void yoshifumistepSE(dActor_c *actor);
    void PlayerFumiJump(dActor_c *actor, float);
    void setFumiComboScore(dActor_c *actor);


    // Nonvirtuals

    void hitdamageEffect(const mVec3_c &pos);
    static void normal_collcheck(dCc_c *cc1, dCc_c *cc2);
    bool getPl_LRflag(const mVec3_c &pos);
    bool getPl_UDflag(const mVec3_c &pos);
    bool carry_check(dActor_c *other);
    void checkWallAndBg(); ///< @unofficial
    int Enfumi_check(dCc_c *cc1, dCc_c *cc2, int step);
    static bool CeilCheck(float, dCc_c *);
    u32 EnBgCheck();
    bool EnBgCheckFoot();
    u32 EnBgCheckWall();
    void WaterCheck(mVec3_c &pos, float h); ///< @unofficial
    bool LineBoundaryCheck(dActor_c *actor);
    dBc_c::WaterCheckResult_e WaterLineProc(const mVec3_c &pos, float h);
    bool EnLavaCheck(const mVec3_c &);
    bool EnWaterCheck(const mVec3_c &);
    bool EnWaterFlagCheck(const mVec3_c &);
    bool Area_X_check(float);
    bool Area_XY_check(float, float);
    bool PlayerCarryCheck(dActor_c *);
    mVec3_c calcCarryPos(const mVec3_c &);
    bool turnangle_calc(const short *, const short *);
    void slipBound(dActor_c *);
    void fireballInvalid(dCc_c *, dCc_c *);
    void iceballInvalid(dCc_c *, dCc_c *);
    void posMove();
    void Bound(float, float, float);
    void boyonInit();
    void setNicePoint_Death();

    block_hit mBlockHit;
    u32 mCcValue;
    u16 m_24;
    u8 mPad1[6];
    mVec3_c mFootRelated;
    mVec3_c mFootRelated2;
    bool blockHitRelated;
    u8 m_45;
    bool m_46, m_47, mFootAttr3;
    u8 m_49;
    bool mFootAttr1;
    u8 mPad3[5];
    dEnBoyoMng_c mBoyoMng;
    u32 mPad4;
    dIceMng_c mIceMng;
    float mSaveAccelY;
    float mSaveSpeedMaxY;
    float mSaveMaxFallSpeed;
    u32 mFlags; // 2: hard hit, 0x1000000: collision related
    CounterCont mCounterCont;
    dEnFumiProc_c mFumiProc;

    static const float smc_WATER_GRAVITY;
    static const float smc_WATER_YMAXSPD;
    static const float smc_WATER_FALLMAXSPD;
    static const float smc_WATER_ROLL_DEC_RATE;
    static const float smc_DEADFALL_GRAVITY[2];
    static const float smc_DEADFALL_YSPEED;
};

extern const s8 l_base_speedX[];
extern const u16 l_base_angleY[];
extern const u16 l_unk_angle[];
