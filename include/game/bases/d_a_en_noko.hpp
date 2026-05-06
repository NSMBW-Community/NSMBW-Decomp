#include <game/bases/d_a_en_shell.hpp>
#include <game/mLib/m_effect.hpp>
#include <lib/MSL/string.h>

/**
 * @brief Koopa Troopa
 * @statetable
 * @paramtable
 */
class daEnNoko_c : public daEnShell_c {
public:
    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        virtual void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl);

        daEnNoko_c *mpOwner;
    };

    enum NokoType_e {
        NOKO_GREEN, ///< Green Koopas don't turn around on ledges.
        NOKO_RED ///< Red Koopas turn around on ledges.
    };

    daEnNoko_c() { mMdlCallback.mpOwner = this; }
    ~daEnNoko_c() {}

    virtual int create() override;
    virtual int doDelete() override;
    virtual int execute() override;
    virtual int preExecute() override;
    virtual int draw() override;
    virtual void finalUpdate() override { calcMdl(); }
    virtual bool createIceActor() override;
    virtual void beginFunsui() override;
    virtual void endFunsui() override;

    STATE_FUNC_DECLARE(daEnNoko_c, BlockAppear);
    STATE_FUNC_DECLARE(daEnNoko_c, Walk);
    STATE_FUNC_DECLARE(daEnNoko_c, Turn);
    STATE_FUNC_DECLARE(daEnNoko_c, WindTurn);
    STATE_FUNC_DECLARE(daEnNoko_c, SpitOut_Ready);
    STATE_FUNC_DECLARE(daEnNoko_c, BgmDance);
    STATE_FUNC_DECLARE(daEnNoko_c, BgmDanceEd);
    STATE_VIRTUAL_FUNC_DECLARE(daEnNoko_c, Wakeup);
    STATE_VIRTUAL_FUNC_DECLARE(daEnNoko_c, WakeupTurn);

    virtual bool setPlayerDamage(dActor_c *actor) override;
    virtual bool checkSleep() override;
    virtual bool turnProc() override;
    virtual void calcShellEffectPos() override;
    virtual void setEnemyTurn() override { if (isState(StateID_Walk)) { changeState(StateID_Turn); } }

    virtual void checkWaterEntry() { WaterCheck(mPos, 1.0f); }

    virtual void setAfterSleepState() override { changeState(StateID_Walk); }
    virtual void slideEffect() override {
        if (!mNokoType) {
            mEffect.createEffect("Wm_en_shellgreentail", 0, &m_71c, nullptr, nullptr);
        } else {
            mEffect.createEffect("Wm_en_shellredtail", 0, &m_71c, nullptr, nullptr);
        }
    }

    virtual void doTurn(int *dir, s16 *turnSpeed);
    virtual bool isWalking() { return true; }
    virtual void turnAround();
    virtual void calcMdl();
    virtual bool canDance();
    virtual void danceWithMove(int move);
    virtual void dance();
    virtual void setInitialState();
    virtual void vf324() {}
    virtual void vf328() {}
    virtual void deleteResExtra() {}
    virtual mVec3_c getPos() { return mVec3_c(mPos.x, mPos.y, mPos.z); }

    virtual BOOL isFunsui() const override { return mIsFrozen; }

    void loadRes();
    void updateAmiLine();
    void setZPos();
    bool isInQuicksand();
    void spawnQuicksandEffects();
    bool playerDamageTurn(dActor_c *);
    void setNokoBc();
    float getWindMultiplier();
    void setMoveAnimation(const char *name, m3d::playMode_e mode, float frame);
    void setBaseAnimation(const char *name, m3d::playMode_e mode, float frame);
    bool checkLedge();
    void landEffect();

    u8 mPad0[4];

    dHeapAllocator_c mNokoAllocator;
    nw4r::g3d::ResFile mNokoResFile;
    m3d::mdl_c mNokoModel;
    m3d::anmChr_c mMoveAnim;
    nw4r::g3d::ResAnmTexPat mNokoResAnmTexPat;
    m3d::anmTexPat_c mNokoAnimTex;

    u32 mIsFrozen;
    s16 mBgmDanceAngle;
    u8 mNokoType; ///< Is a NokoType_e.
    mVec3_c mCreatePos;
    float mXSpeedBeforeFrozen;
    u8 mPad1[8];
    float mBaseZPos;
    u32 m_8c8;
    int mDancesRemaining;
    mAng mBgmDanceRotSpeed;
    u32 mDanceMove;
    mEf::levelOneEffect_c mQuickSandEffect;
    nodeCallback_c mMdlCallback;

    ACTOR_PARAM_CONFIG(NokoType, 0, 1);
    ACTOR_PARAM_CONFIG(SpawnMode, 4, 1); ///< 1 = Spawn as sleeping shell
    ACTOR_PARAM_CONFIG(Layer, 16, 1);
    ACTOR_PARAM_CONFIG(BlockHitPlayer, 24, 2);
    ACTOR_PARAM_CONFIG(BlockAppear, 28, 1);
    ACTOR_PARAM_CONFIG(SpitOut, 29, 1);
};
