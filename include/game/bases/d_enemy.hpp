#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_en_boyo_manager.hpp>
#include <game/bases/d_ice_manager.hpp>
#include <constants/game_constants.h>

/// @unofficial
struct sDeathInfoData {
    float mXSpeed;
    float mYSpeed;
    float mMaxYSpeed;
    float mYAccel;
    sStateIDIf_c *mDeathState;
    int mQuakeScore;
    int m_18;
    bool mMovingLeft;
    s8 mKilledBy;
    bool mIsDead;
};

/// @unofficial
class dDeathInfo_c {
public:
    dDeathInfo_c() {
        mData.mIsDead = false;
    }

    void set(const sDeathInfoData &other) {
        mData.mIsDead = true;
        float tmpY = other.mYSpeed;
        mData.mXSpeed = other.mXSpeed;
        mData.mYSpeed = tmpY;
        mData.mMaxYSpeed = other.mMaxYSpeed;
        mData.mYAccel = other.mYAccel;
        mData.mDeathState = other.mDeathState;
        mData.mQuakeScore = other.mQuakeScore;
        mData.m_18 = other.m_18;
        mData.mMovingLeft = other.mMovingLeft;
        mData.mKilledBy = other.mKilledBy;
    }

    sDeathInfoData mData;
};

/// @brief An enemy actor.
/// Note that the definition of "enemy" in this context is actually
/// any interactible stage entity, not just ones that can hurt the player.
/// @ingroup bases
class dEn_c : public dActorMultiState_c {
public:
    /// @unofficial
    enum CLAP_MODE_e {
        CLAP_0,
        CLAP_1,
        CLAP_2
    };

    /// @unofficial
    enum FLAGS_e {
        FLAG_1 = BIT_FLAG(1),
        FLAG_24 = BIT_FLAG(24)
    };

    dEn_c(); ///< Constructs a new enemy actor.
    virtual ~dEn_c(); ///< @copydoc dActorMultiState_c::~dActorMultiState_c

    // Base class overrides

    virtual void postCreate(fBase_c::MAIN_STATE_e status);
    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual int preDraw();

    virtual void block_hit_init();

    virtual void setEatTongue(dActor_c*);
    virtual void setEatTongueOff(dActor_c*);
    virtual bool setEatSpitOut(dActor_c*);

    virtual void calcEatInScale(dActor_c *);

    virtual void yoganSplashEffect(const mVec3_c&, float);
    virtual void poisonSplashEffect(const mVec3_c&, float);

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

    virtual void setDeathInfo_Other(dActor_c *killedBy);
    virtual void setDeathInfo_Quake(int);
    virtual void setDeathInfo_IceBreak();
    virtual void setDeathInfo_IceVanish();
    void setDeathInfo_Smoke(dActor_c *killedBy);
    void setDeathInfo_Fumi(dActor_c *killedBy, mVec2_c, const sStateIDIf_c &, int);
    void setDeathInfo_YoshiFumi(dActor_c *killedBy);
    void setDeathInfo_SpinFumi(dActor_c *killedBy, int);

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

    void boyonInit();
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

    void checkWallAndBg(); ///< @unofficial
    int Enfumi_check(dCc_c *cc1, dCc_c *cc2, int step);
    u32 EnBgCheck();
    bool EnBgCheckFoot();
    u32 EnBgCheckWall();
    bool LineBoundaryCheck(dActor_c *actor);

    void WaterCheck(mVec3_c &pos, float h); ///< @unofficial
    dBc_c::WaterCheckResult_e WaterLineProc(const mVec3_c &pos, float h);
    bool EnLavaCheck(const mVec3_c &pos);
    bool EnWaterCheck(const mVec3_c &pos);
    bool EnWaterFlagCheck(const mVec3_c &pos);

    bool getPl_LRflag(const mVec3_c &pos); ///< Checks whether the nearest player is to the left of @p pos.
    bool getPl_UDflag(const mVec3_c &pos); ///< Checks whether the nearest player is below @p pos.
    bool Area_X_check(float x);
    bool Area_XY_check(float x, float y);
    bool carry_check(dActor_c *other);
    bool PlayerCarryCheck(dActor_c *actor);
    mVec3_c calcCarryPos(const mVec3_c &pos);

    void Bound(float epsY, float scaleX, float scaleY);
    void slipBound(dActor_c *actor);
    void posMove();

    void fireballInvalid(dCc_c *cc1, dCc_c *cc2);
    void iceballInvalid(dCc_c *cc1, dCc_c *cc2);

    bool turnangle_calc(const short *target, const short *delta);
    void setNicePoint_Death();

    static void normal_collcheck(dCc_c *cc1, dCc_c *cc2);
    static bool CeilCheck(float y, dCc_c *cc);

    dDeathInfo_c mDeathInfo; ///< The parameters for the death animation.
    u32 mCcValue;
    u16 m_24; ///< @unused
    u8 mPad1[6];
    mVec3_c mFootPush;
    mVec3_c mFootPush2;
    bool mDeathFallDirection; ///< The X direction to move towards on death.
    u8 mPad2[1];
    bool mKilledByLiquid; ///< Whether the enemy was killed by falling in a liquid.
    u8 mPad3[1];
    bool mFootAttr3;
    bool mInLiquid; ///< Whether the enemy is in a liquid.
    bool mFootAttr1;
    u8 mPad4[5];
    dEnBoyoMng_c mBoyoMng;
    u8 mPad5[4];
    dIceMng_c mIceMng; ///< The ice manager for this enemy.
    float mAirAccelY; ///< The Y acceleration before entering a liquid.
    float mAirSpeedMaxY; ///< The maximum Y speed before entering a liquid.
    float mAirMaxFallSpeed; ///< The maximum fall speed before entering a liquid.
    u32 mFlags; ///< Flags for this actor. See FLAGS_e.
    u16 mTimer1; ///< [Used in EN_HATENA_BALLON, for example]
    u16 mTimer2; ///< [Used in EN_HATENA_BALLON, for example]
    /// @brief Hit cooldown timers for each player.
    /// This is used to prevent, for example, a thrown shell from hitting
    /// the player that threw it.
    u16 mPlayerNoHitCooldown[PLAYER_COUNT];
    u8 mTimer3; /// @todo Unused?
    CLAP_MODE_e mClapMode;
    dEnFumiProc_c mFumiProc;

    static const float smc_WATER_GRAVITY;
    static const float smc_WATER_YMAXSPD;
    static const float smc_WATER_FALLMAXSPD;
    static const float smc_WATER_ROLL_DEC_RATE;
};

extern const s8 l_base_speedX[]; ///< @unofficial
extern const s16 l_base_angleY[];
extern const s16 l_unk_angle[]; ///< @unofficial
extern const float l_dirX[]; ///< @unofficial
