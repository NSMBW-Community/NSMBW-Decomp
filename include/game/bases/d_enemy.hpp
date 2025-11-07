#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_en_boyo_manager.hpp>
#include <game/bases/d_en_combo.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_ice_manager.hpp>
#include <constants/game_constants.h>

/// @unofficial
struct sDeathInfoData {
    float mXSpeed;
    float mYSpeed;
    float mMaxYSpeed;
    float mYAccel;
    const sStateIDIf_c *mDeathState;
    int mScore;
    int m_18;
    u8 mDirection;
    u8 mKilledBy;
};

/// @unofficial
class dDeathInfo_c {
public:
    dDeathInfo_c() : mIsDead(false) {}

    dDeathInfo_c &operator=(const sDeathInfoData &other) {
        mIsDead = true;
        mSpeed.set(other.mXSpeed, other.mYSpeed);
        mMaxYSpeed = other.mMaxYSpeed;
        mYAccel = other.mYAccel;
        mDeathState = other.mDeathState;
        mScore = other.mScore;
        m_18 = other.m_18;
        mDirection = other.mDirection;
        mKilledBy = other.mKilledBy;
        return *this;
    }

    float getXSpeed() const {
        return mSpeed.x;
    }

    float getYSpeed() const {
        return mSpeed.y;
    }

    float getMaxYSpeed() const {
        return mMaxYSpeed;
    }

    float getYAccel() const {
        return mYAccel;
    }

    static const u8 smc_UNKNOWN_HIT = 50; ///< @unofficial

private:
    mVec2_c mSpeed;
    float mMaxYSpeed;
    float mYAccel;
public:
    const sStateIDIf_c *mDeathState;
    int mScore;
    int m_18;
    u8 mDirection;
    s8 mKilledBy;
    bool mIsDead;
};

/// @unofficial
class dPlayerDownTimer_c {
public:
    void update(int idx) {
        if (mTimer[idx] != 0) {
            mTimer[idx]--;
        }
    }

    u16 mTimer[PLAYER_COUNT];
};

/// @brief An enemy actor.
/// Note that the definition of "enemy" in this context is actually
/// any interactible stage entity, not just ones that can hurt the player.
/// @ingroup bases
class dEn_c : public dActorMultiState_c {
public:
    /// @unofficial
    enum FLAGS_e {
        EN_IS_SHELL = BIT_FLAG(0),
        EN_IS_HARD = BIT_FLAG(1),
        EN_FLAG_24 = BIT_FLAG(24)
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

    virtual bool EnDamageCheck(dCc_c *self, dCc_c *other);
    virtual bool PlDamageCheck(dCc_c *self, dCc_c *other);
    virtual bool YoshiDamageCheck(dCc_c *self, dCc_c *other);
    virtual bool EtcDamageCheck(dCc_c *self, dCc_c *other);

    virtual void Normal_VsEnHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsPlHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other);

    virtual bool hitCallback_Star(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Slip(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Large(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Spin(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Rolling(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_WireNet(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_HipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_YoshiHipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Screw(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_PenguinSlide(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Cannon(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Shell(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Fire(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Ice(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_YoshiBullet(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_YoshiFire(dCc_c *self, dCc_c *other);

    virtual void setDeathInfo_Other(dActor_c *killedBy);
    virtual void setDeathInfo_Quake(int);
    virtual void setDeathInfo_IceBreak();
    virtual void setDeathInfo_IceVanish();
    void setDeathInfo_Smoke(dActor_c *killedBy);
    void setDeathInfo_Fumi(dActor_c *killedBy, mVec2_c, const sStateIDIf_c &, int);
    void setDeathInfo_YoshiFumi(dActor_c *killedBy);
    void setDeathInfo_SpinFumi(dActor_c *killedBy, int);

    virtual bool isQuakeDamage();
    virtual void hitYoshiEat(dCc_c *self, dCc_c *other);

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

    virtual void beginFunsui();
    virtual void endFunsui();
    virtual void isFunsui() const;

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

    bool checkWallAndBg(); ///< @unofficial
    int Enfumi_check(dCc_c *self, dCc_c *other, int step);
    u32 EnBgCheck();
    bool EnBgCheckFoot();
    u32 EnBgCheckWall();
    bool LineBoundaryCheck(dActor_c *actor);

    void WaterCheck(mVec3_c &pos, float h); ///< @unofficial
    dBc_c::WATER_TYPE_e WaterLineProc(const mVec3_c &pos, float h);
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
    bool turnangle_calc(const short *target, const short *delta);

    void fireballInvalid(dCc_c *self, dCc_c *other);
    void iceballInvalid(dCc_c *self, dCc_c *other);

    void setNicePoint_Death();
    void setDeadMode(dActor_c *actor, int);
    void killIce();

    static void normal_collcheck(dCc_c *self, dCc_c *other);
    static bool CeilCheck(float y, dCc_c *cc);

    u8 getDeathFallDirection() const { return mDeathFallDirection; }

    dDeathInfo_c mDeathInfo; ///< The parameters for the death animation.
    u32 mCcValue;
    u16 m_24; ///< @unused
    u8 mPad1[6];
    mVec3_c mFootPush;
    mVec3_c mFootPush2;
    u8 mDeathFallDirection; ///< The X direction to move towards on death.
    u8 mIceDeathDirection; ///< The X direction to move towards on ice death.
    bool mKilledByLiquid; ///< Whether the enemy was killed by falling in a liquid.
    u8 mPad2[1];
    bool mFootAttr3;
    bool mInLiquid; ///< Whether the enemy is in a liquid.
    bool mFootAttr1;
    u8 mPad3[5];
    dEnBoyoMng_c mBoyoMng;
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
    dPlayerDownTimer_c mNoHitPlayer;

    u8 mTimer3; /// @todo Unused?
    dEnCombo_c mCombo; ///< The enemy combo manager.
    dEnFumiProc_c mFumiProc;

    static const float smc_WATER_GRAVITY;
    static const float smc_WATER_YMAXSPD;
    static const float smc_WATER_FALLMAXSPD;
    static const float smc_WATER_ROLL_DEC_RATE;

    static const float smc_DEADFALL_GRAVITY;
    static const float smc_DEADFALL_YSPEED;
    static const float smc_DEADFALL_YSPEED_MAX;
    static const s16 smc_DEADFALL_SPINSPEED;

    static const u16 smc_NO_HIT_PLAYER_TIMER_DEFAULT = 5; ///< @unofficial
    static const u16 smc_NO_HIT_PLAYER_TIMER_SPIT_OUT = 16; ///< @unofficial
};

extern const s8 l_EnMuki[]; ///< Multiplier for directions.
extern const s16 l_base_angleY[]; ///< Default Y rotation angles per direction.
extern const s16 l_base_angleY_add[]; ///< Default Y rotation animation increments per direction.
extern const float l_base_fall_speed_x[]; ///< Default X fall speeds for death animation per direction.
