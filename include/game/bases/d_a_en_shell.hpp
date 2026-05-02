#include <game/bases/d_enemy.hpp>
#include <game/bases/d_a_en_carry.hpp>
#include <game/bases/d_ef.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_effect.hpp>

class daEnShell_c : public daEnCarry_c {
public:
    struct GlobalData_t {
        s16 mSpinSpeed;
        s16 mUnkSpeed;
        float mSlideSpeedPowerup;
        float mSlideSpeedNoPowerup;
    };

    daEnShell_c();
    virtual ~daEnShell_c();

    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual bool ActorDrawCullCheck();
    virtual void block_hit_init();
    virtual void boyonBegin() {}
    virtual bool isSpinLiftUpEnable() { return isState(StateID_Sleep) != 0; }
    virtual void setEatTongue(dActor_c *actor);
    virtual bool setEatSpitOut(dActor_c *actor);
    virtual void setSpinLiftUpActor(dActor_c *carryingActor);
    virtual void setCarryFall(dActor_c *carryingActor, int collisionDelay);
    virtual bool EtcDamageCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsEnHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsPlHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Spin(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_HipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_YoshiHipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Shell(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Ice(dCc_c *self, dCc_c *other);
    virtual bool setDamage(dActor_c *actor);
    virtual void returnState_Ice();
    virtual bool checkComboClap(int max);
    virtual void FumiJumpSet(dActor_c *actor);
    virtual void FumiScoreSet(dActor_c *actor);
    virtual void YoshiFumiScoreSet(dActor_c *actor) { daEnShell_c::FumiScoreSet(actor); }

    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, DieFall);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Carry);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Sleep);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Slide);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Wakeup);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, WakeupReverse);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, WakeupTurn);

    virtual bool setPlayerDamage(dActor_c *actor) { return true; }
    virtual bool checkSleep();
    virtual void setEnemyTurn() {}
    virtual bool specialFumiProc(dActor_c *actor) { return false; }
    virtual bool specialFumiProc_Yoshi(dActor_c *actor) { return specialFumiProc(actor); }
    virtual bool isFumiInvalid() const { return false; }
    virtual void setAfterSleepState() {}
    virtual bool turnProc() { return false; }
    virtual void kickSE(int plrNo);
    virtual void kickEffect(mVec3_c pos) { dEf::createEffect_change("Wm_mr_kickhit", 0, &pos, nullptr, nullptr); }
    virtual bool isDieShell() { return true; }
    virtual void calcShellEffectPos() {}
    virtual void slideEffect() {}
    virtual bool isBlockHitDeath() const { return false; }

    /// @brief Loads the resources for the shell model and animation.
    /// @param arcName The name of the resource archive to load from.
    /// @param resPath The path within the archive to the model resource.
    /// @param modelName The name of the model resource.
    /// @param anmTexName The name of the texture animation resource (optional).
    /// @param animFrame The frame of the texture animation to start at.
    void createShell(const char *arcName, const char *resPath, const char *modelName, const char *anmTexName, float animFrame);
    void calcShellMdl();
    bool drawShell();
    void setKickSlide(dCc_c *self, dActor_c *other);
    bool fn_800397b0(dActor_c *actor); ///< @unofficial
    bool checkMugenCombo(dActor_c *actor);
    bool cullCheck();
    bool cullCheck_Shell();
    void setSlideThrowSpeed(dActor_c *actor);
    void setSlideKickSpeed(dActor_c *actor);
    void slideSpin();
    void setDeathInfo_CarryBgIn(dActor_c *);
    void adjustCarryCc();
    bool hasamiCheck();
    void setDeathInfo_Hasami();

    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mResFile;
    m3d::mdl_c mModel;
    m3d::anmChr_c mAnim;
    nw4r::g3d::ResAnmTexPat mResAnmTexPat;
    m3d::anmTexPat_c mAnimTex;
    u8 mPad1[0x24];
    sBcSensorPoint mSensor0;
    sBcSensorLine mSensor1;
    sBcSensorLine mSensor2;
    sBcSensorLine mSensor3;
    mEf::levelEffect_c mEffect;
    int m_254;
    int m_258;
    u8 mPad2[0xc];
    mVec3_c mCarryPos;
    int m_274;
    int m_278;
    mVec2_c mCcOffset;
    mVec2_c mCcSize;
    fBaseID_e m_28c;
    int m_290;
    mAng mWakeupShakeAngle;
    mAng3_c mWakeupShakeAngle3D;
    u16 mSleepTimer; ///< How many frames longer the shell will remain asleep.
    u16 mCarryTimer; ///< How many more frames the sleep timer will remain paused while being carried.
    bool mIsFlipped; ///< Whether the shell is flipped upside down from a block hit.
    int mCarriedBy;
    int mIsMugenCombo;
    float mMugenComboSpeed;
    float mMugenComboPosX;

    ACTOR_PARAM_CONFIG(MugenRelated, 20, 1);

    // Unofficial constants

    static const u16 smc_SLEEP_TIMER = 511;
    static const u16 smc_SLEEP_TIMER_SHAKE = 96;
};
