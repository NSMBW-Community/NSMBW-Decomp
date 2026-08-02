#pragma once

#include <game/bases/d_enemy_boss.hpp>
#include <game/mLib/m_effect.hpp>

class dEnTorideKokoopa_c : public dEnBoss_c {
public:
    /// @unofficial
    struct VoiceInfo_s {
        dAudio::SoundEffectID_t mSoundID;
        float mAnmFrame;
    };

    /// @unofficial
    struct VoicesInfo_s {
        VoiceInfo_s mEntries[11];
    };

    /// @unofficial
    struct JumpAnmData_s {
        const char *mJumpStName;
        const char *mJumpName;
        const char *mBigJumpStName;
        const char *mBigJumpName;
        const char *mLandOnName;
        mVec2_c mJumpSpeed0;
        mVec2_c mBigJumpSpeed0;
        mVec2_c mJumpSpeed1;
        mVec2_c mBigJumpSpeed1;
    };

    /// @unofficial
    struct DemoAnmData_s {
        const char *mWaitName;
        const char *mAwakeName;  ///< Plays notice1Vo only when not nullptr
        const char *mAwakeWaitName;
        const char *mIkakuName;  ///< Plays notice2Vo only when not nullptr
        const char *mIkakuWaitName;
    };

    /// @unofficial
    enum DrawFlags_e {
        DRAW_NONE = 0,
        DRAW_KOKOOPA = BIT_FLAG(0),
        DRAW_SHELL = BIT_FLAG(1),
        DRAW_BOTH = DRAW_KOKOOPA | DRAW_SHELL
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
    int isFumiInvalid() const override { return mDrawFlags & DRAW_SHELL; }
    int isFireInvalid() const override { return mDrawFlags & DRAW_SHELL; }
    int isStarInvalid() const override { return mDrawFlags & DRAW_SHELL; }
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
    virtual float vf4d0() = 0; ///< @unofficial
    virtual mVec3_c getMagicStickEffectOffset() const { return mVec3_c(0.0f, 0.0f, 18.0f); }
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

    dActor_c *getBlitz() {
        fBaseID_e id = m_70c;
        if (id == BASE_ID_NULL) {
            return nullptr;
        }
        return (dActor_c *) fManager_c::trySearchBaseByID(id);
    }


    u8 mPad_598[4];
    nw4r::g3d::ResFile mResFile;
    m3d::mdl_c mModel;
    m3d::anmChr_c mAnmChr;
    m3d::anmMatClr_c mAnmMatClr;
    nw4r::g3d::ResAnmTexPat mResAnmTexPat;
    m3d::anmTexPat_c mAnmTexPat;
    m3d::mdl_c mShellModel;
    m3d::anmChr_c mShellAnmChr;
    const char **m_6ec; //< TODO: make a structure out of this
    JumpAnmData_s *mJumpAnmInfo;
    const char **m_6f4; //< TODO: make a structure out of this
    const char **m_6f8; //< TODO: make a structure out of this
    DemoAnmData_s *mDemoAnmInfo;
    u32 mAttackTargetPlayerId;
    int mAttackSearchTimer;
    u8 mPad_708[4];
    fBaseID_e m_70c;
    mVec3_c mBlitzPos;
    u32 m_71c;
    mVec3_c mFacePos;
    s16 mCurLookAngle;
    s16 mLookAngle;
    int mDrawFlags; ///< Is a DrawFlags_e.
    int mAtkCnt;
    dCc_c mWandCc;
    float mArenaEdgeOffset[2]; ///< Indexed by mDirection. Horizontal distance measured from the display center.
    int mNumJumps;
    int mFireLoopEffectTimer;
    mEf::levelEffect_c mLevelEffect;
    mEf::levelEffect_c mLevelEffect2;
    int mNumLandFramesLeft;
    mVec3_c mPressScale;
    mVec3_c mRecoverScaleStep;
    u8 mPad_a58[4];
    s16 mShellRotStep; ///< Angle by which to rotate the shell during shell attacks. Incremented by 0x200 each frame, capped at 0x1800.
    u8 mPad_a5e[2];
    float mShellAtkSweepStartXPos; ///< The x-position at which a sweep starts.
    int mShellAtkRemNumSweeps; ///< The number of full-width sweeps in shell form remaining during the ShellAtk state.
    float m_a68;
    int mRootJntIdx;
    int mShellJntIdx;
    mVec3_c mRootJntPos;
    mVec3_c mShellJntPos;
    char **mEffectNames;
    mEf::levelEffect_c mLevelEffects[3];
    VoicesInfo_s *mpVoicesInfo;
};
