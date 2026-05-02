#include <game/bases/d_enemy.hpp>
#include <game/bases/d_a_en_carry.hpp>
#include <game/bases/d_ef.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_effect.hpp>

class daEnShell_c : public daEnCarry_c {
public:
    daEnShell_c();
    virtual ~daEnShell_c();

    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual bool ActorDrawCullCheck();
    virtual void block_hit_init();
    virtual void setEatTongue(dActor_c *actor);
    virtual bool setEatSpitOut(dActor_c *actor);
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
    virtual void setAfterSleepState();
    virtual void turnProc();
    virtual void kickSE(int plrNo);
    virtual void kickEffect(mVec3_c pos) { dEf::createEffect_change("Wm_mr_kickhit", 0, &pos, nullptr, nullptr); }
    virtual void isDieShell();
    virtual void calcShellEffectPos() {}
    virtual void slideEffect();
    virtual bool isBlockHitDeath() const { return false; }

    void createShell(const char *s1, const char *s2, const char *s3, const char *s4, float f);
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

    s16 getSpinSpeed() const { return msc_SPIN_SPEED; }

    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mResFile;
    m3d::mdl_c mModel;
    m3d::anmChr_c mAnim;
    nw4r::g3d::ResAnmTexPat mResAnmTexPat;
    m3d::anmTexPat_c mAnimTex;
    u8 mPad1[0x60];
    mEf::levelEffect_c mEffect;
    int m_254;
    int m_258;
    u8 mPad2[0xc];
    mVec3_c m_268;
    int m_274;
    u8 mPad3[0x4];
    mVec2_c mCcOffset;
    mVec2_c mCcSize;
    fBaseID_e m_28c;
    int m_290;
    mAng m_294;
    mAng3_c m_296;
    u16 m_29c;
    u16 m_29e;
    bool m_2a0;
    int mCarriedBy;
    int m_2a8;
    float m_2ac;
    float m_2b0;

    ACTOR_PARAM_CONFIG(MugenRelated, 20, 1);

    static const s16 msc_SPIN_SPEED; ///< @unofficial
};
