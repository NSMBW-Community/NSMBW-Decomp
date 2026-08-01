#pragma once

#include <game/bases/d_enemy_boss.hpp>
#include <game/mLib/m_effect.hpp>

class dEnTorideKokoopa_c : public dEnBoss_c {
public:
    /// @unofficial
    struct VoiceInfo_s {
        SOUND_e mSoundID;
        float mAnmFrame;
    };

    /// @unofficial
    struct VoicesInfo_s {
        VoiceInfo_s mEntries[11];
    };

    dEnTorideKokoopa_c();
    virtual ~dEnTorideKokoopa_c();

    int preExecute() override;
    void postExecute(fBase_c::MAIN_STATE_e status) override;
    int draw() override;

    void finalUpdate() override;
    mVec2_c getLookatPos() const override { return mVec2_c(mFacePos.x, mFacePos.y); }

    bool hitCallback_PenguinSlide(dCc_c *self, dCc_c *other) override;
    bool isQuakeDamage() override;

    void setBattleReady() override;
    void tenmetsuProc() override;
    void tenmetsuFin() override;
    int getTenmetsuTime_Fire() override { return 24; }
    int getTenmetsuTime_Press() override { return 24; }
    void setFumiDamage(dActor_c *) override;
    void setFumiDead(dActor_c *) override;
    void setFireDamage(dActor_c *) override;
    void setFireDead(dActor_c *) override;
    void setStarDamage(dActor_c *) override;
    void setStarDead(dActor_c *) override;
    void setQuakeDamage() override;
    void setQuakeDead() override;
    void setShellDamage(dActor_c *) override;
    void setShellDead(dActor_c *) override;
    void damageProc() override;
    void deadProc() override;
    int isFumiInvalid() const override { return m_730 & 2; }
    int isFireInvalid() const override { return m_730 & 2; }
    int isStarInvalid() const override { return m_730 & 2; }
    void fumideadEffect() override;
    void fumidmgEffect() override;
    void damageSVo() override;
    void damageLVo() override;

    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, Jump_St);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, BigJump_St);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, BigJump);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, LandOn);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, AttackReady);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, AttackBegin);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, AttackSearch);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, Attack);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, AttackEnd);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, FumiHit);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, FireHit);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, SlideHit);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, StarHit);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, QuakeHit);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, ShellHit);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, ShellAtk_St);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, ShellAtk);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, ShellOut);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DieFumi_St);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DemoWait);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DieShell);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DieFire);

    virtual BOOL lockonTurn();
    virtual void calcKokoopaMdl() {}
    virtual void calcShellMdl() {}
    virtual void drawKokoopa() {}
    virtual void drawShell() {}
    virtual void setBeginMoveState();
    virtual void moveAdjust_HIO() {}
    virtual u32 getAtkEndTime() { return 0; }
    virtual u32 getAtkEndTime_Wait() { return 0; }
    virtual s16 getAtkSearchTime() { return 0; }
    virtual s16 getAtkSearch2ndTime() { return 0; }
    virtual int getDownTime() { return 50; }
    virtual void setAtkCnt();
    virtual float getJumpGravity() { return -0.1875f; }
    virtual float getDrawScale() { return 1.0f; }
    virtual void speedUp() {}
    virtual bool beginDance() { return false; }
    virtual s16 getTurnSpeed();
    virtual int getFumiRecoverTime() { return 4; }
    virtual fBaseID_e createBlitz() { return BASE_ID_NULL; }
    virtual float vf4d0() = nullptr; ///< @unofficial
    virtual mVec3_c getMagicStickEffectOffset() const;
    virtual void setKokoopaCc() {}
    virtual void setShellCc() {}
    virtual float getJumpDist() const { return 64.0f; }
    virtual mVec3_c calcBlitzPos();
    virtual void blitzShoot() {}
    virtual void setBlitzTarget() {}
    virtual mVec3_c calcFacePos() { return mPos; }
    virtual void calcCcData() {}
    virtual void calcWandCcData() {}
    virtual float getKokoopaOffFrm() const { return 0.0f; }
    virtual float getShellOnFrm() const { return 0.0f; }
    virtual float getKokoopaOnFrm() const { return 0.0f; }
    virtual float getShellOffFrm() const { return 0.0f; }
    virtual bool checkGetUp() const { return false; }
    virtual float getCreateBlitzFrm() const { return 0.0f; }
    virtual float getShootFrm() const { return 0.0f; }
    virtual mVec3_c getPressScale() { return mVec3_c(1.8f, 1.0f, 1.2f); }
    virtual u32 getPressTime() { return 20; };
    virtual s16 defaultDirAngle() { return 0x2000; }
    virtual float getShellChangeEffectOffsetY() const { return 10.0f; }
    virtual void jumpEffect();
    virtual void jumpRootEffect();
    virtual void landonEffect();
    virtual void shellLandonEffect();
    virtual void hitFireLoopEffect();
    virtual void hitFireDamageEffect();
    virtual void shellChangeEffect();
    virtual void shellBumMarEffect();
    virtual void shellAtkEffect();
    virtual void downFallEffect();
    virtual void downLandOnEffect(float scale);
    virtual void hitShellDamageEffect() {}
    virtual void ikakuEffect() {}
    virtual void jumpSE() { mSndObject.startSound(SE_BOSS_CMN_JUMP_M, m_d0, 0); }
    virtual void landonSE() { mSndObject.startSound(SE_BOSS_CMN_SMART_LAND, m_d0, 0); }
    virtual void shelllandonSE() { mSndObject.startSound(SE_BOSS_CMN_SMART_SHELL_LAND, m_d0, 0); }
    virtual void shellinSE() { mSndObject.startSound(SE_BOSS_CMN_SHELL_IN, m_d0, 0); }
    virtual void shelloutSE() { mSndObject.startSound(SE_BOSS_CMN_SHELL_OUT, m_d0, 0); }
    virtual void shellatkSE() { mSndObject.holdSound(SE_BOSS_CMN_SMART_SHELL_ROLL, m_d0, 0); }
    virtual void getupSE() {}
    virtual void blitzchargeSE() {}
    virtual void notice1Vo();
    virtual void notice2Vo();
    virtual void wakeVo();
    virtual void escJumpVo();
    virtual void magicShotVo();
    virtual void shellOutVo();
    virtual void deadVo();
    virtual void loseFirstVo();
    virtual void loseSecondVo();
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DemoAwake);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DemoAwake_Wait);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DemoIkaku);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DemoIkaku_Wait);
    STATE_VIRTUAL_FUNC_DECLARE(dEnTorideKokoopa_c, DemoEscape_St);
    virtual void awakeSE() {}
    virtual void ikakuSE() {}

    void calcRootJntPos();
    void calcShellJntPos();
    float calcJumpRate();
    bool movelimitCheck(float);
    void moveRevise();
    static void wandCcCallback(dCc_c * self, dCc_c * other);
    int calcAttackTarget();
    void calcLookAngle();
    void changeShell();
    void changeKokoopa();
    u32 getTorideFunfareTime();
    BOOL calcDirAngle(s16 speed);
    void blitzMove(dActor_c * other);
    bool isTorideBoss();
    void shellWallEffect();
    bool checkDownJump();
    bool isCreateBlitz() const;
    bool isShootBlitz() const;

    u8 mPad_598[4];
    nw4r::g3d::ResFile mResFile;
    m3d::mdl_c mModel;
    m3d::anmChr_c mAnmChr;
    m3d::anmMatClr_c mAnmMatClr;
    nw4r::g3d::ResAnmTexPat m_644;
    m3d::anmTexPat_c mAnmTexPat;
    m3d::mdl_c mModel2;
    m3d::anmChr_c mAnmChr2;
    char **m_6ec; //< TODO: make a structure out of this
    char **mJumpAnmNames; //< TODO: make a structure out of this
    char **m_6f4; //< TODO: make a structure out of this
    char **m_6f8; //< TODO: make a structure out of this
    char **m_6fc; //< TODO: make a structure out of this
    u32 m_700;
    int m_704;
    u8 mPad_708[4];
    fBaseID_e m_70c;
    mVec3_c mBlitzPos;
    u32 m_71c;
    mVec3_c mFacePos;
    s16 m_72c;
    s16 mLookAngle;
    int m_730;
    int mAtkCnt;
    dCc_c mCc2;
    float m_7dc[2];
    int m_7e4;
    int m_7e8;
    mEf::levelEffect_c mLevelEffect;
    mEf::levelEffect_c mLevelEffect2;
    int m_a3c;
    mVec3_c m_a40;
    mVec3_c m_a4c;
    u8 mPad_a58[4];
    s16 m_a5c;
    u8 mPad_a5e[2];
    float m_a60;
    int m_a64;
    float m_a68;
    u32 m_a6c;
    u32 m_a70;
    mVec3_c mRootJntPos;
    mVec3_c mShellJntPos;
    char **mEffectNames;
    mEf::levelEffect_c mLevelEffects[3];
    VoicesInfo_s *mpVoicesInfo;
};
