#include <game/bases/d_enemy.hpp>
#include <game/bases/d_a_en_carry.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_effect.hpp>

class daEnShell_c : public daEnCarry_c {
public:
    daEnShell_c();
    virtual ~daEnShell_c();

    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual bool EtcDamageCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsEnHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsPlHitCheck(dCc_c *self, dCc_c *other);

    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, DieFall);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Carry);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Sleep);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Slide);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, Wakeup);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, WakeupReverse);
    STATE_VIRTUAL_FUNC_DECLARE(daEnShell_c, WakeupTurn);

    virtual void setPlayerDamage();
    virtual void checkSleep();
    virtual void setEnemyTurn() {}
    virtual void specialFumiProc();
    virtual void specialFumiProc_Yoshi();
    virtual void isFumiInvalid();
    virtual void setAfterSleepState();
    virtual void turnProc();
    virtual void kickSE();
    virtual void kickEffect();
    virtual void isDieShell();
    virtual void calcShellEffectPos() {}
    virtual void slideEffect();
    virtual void isBlockHitDeath();

    void createShell(const char *s1, const char *s2, const char *s3, const char *s4, float f);
    void calcShellMdl();
    bool drawShell();
    bool hasamiCheck();
    void setDeathInfo_Hasami();

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
    u8 mPad3[0x14];
    fBaseID_e m_28c;
    int m_290;
    u8 mPad4[0x2];
    mAng3_c m_296;
    u16 m_29c;
    u8 mPad5[0x2];
    bool m_2a0;
    u8 mPad6[0x4];
    int m_2a8;
    float m_2ac;
};
