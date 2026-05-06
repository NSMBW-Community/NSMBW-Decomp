#include <game/bases/d_a_en_shell.hpp>
#include <lib/MSL/string.h>

namespace nw4r {
    namespace ef {
        struct EmitterInheritSetting {
            u16 mSpeed;
            u8 mScale;
            u8 mAlpha;
            u8 mColor;
            u8 mWeight;
            u8 mType;
            u8 mFlag;
            u8 mAlphaFuncPri;
            u8 mAlphaFuncSec;
            u8 mPad[2];
        };
    };
};

namespace mEf {
    class levelOneEffect_c : public levelEffect_c {
    public:
        levelOneEffect_c() { reset(); }

        float mEmissionRateMaybe;
        nw4r::ef::EmitterInheritSetting setting;
    };
};

class daTagWind_c : public dActor_c {
public:
    float m_394;
    u8 mPad[32];
};

/**
 * @brief Koopa Troopa
 * @statetable
 * @paramtable
 */
class daEnNoko_c : public daEnShell_c {
public:
    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        virtual void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) {
            mMtx_c mtx;
            if (strcmp(resMdl.GetResNode(nodeId).GetName(), "head") == 0) {
                manip->GetMatrix(&mtx);
                mtx.XrotM(mpOwner->m_8a8);
                manip->SetMatrix(mtx);
            }
        }

        daEnNoko_c *mpOwner;
    };

    daEnNoko_c() { mMdlCallback.mpOwner = this; }
    ~daEnNoko_c() {}

    virtual int create() override;
    virtual int doDelete() override;
    virtual int execute() override;
    virtual int preExecute() override;
    virtual int draw() override;
    virtual void finalUpdate() override { vf310(); }
    virtual bool createIceActor() override;
    virtual void beginFunsui() override;
    virtual void endFunsui() override;
    virtual BOOL isFunsui() const override { return mIsFrozen; }
    virtual void YoshiFumiScoreSet(dActor_c *) override;

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
    virtual void calcShellEffectPos() override;
    virtual void setEnemyTurn() override { if (isState(StateID_Walk)) { changeState(StateID_Turn); } }
    virtual void setAfterSleepState() override { changeState(StateID_Walk); }
    virtual void slideEffect() override {
        if (!mWalksOffLedges) {
            mEffect.createEffect("Wm_en_shellgreentail", 0, &m_71c, nullptr, nullptr);
        } else {
            mEffect.createEffect("Wm_en_shellredtail", 0, &m_71c, nullptr, nullptr);
        }
    }

    virtual void vf300() { WaterCheck(mPos, 1.0f); }
    virtual void vf304(u32 * pDir, mAng * pAng);
    virtual bool isWalking();
    virtual void vf30C();
    virtual void vf310();
    virtual bool canDance();
    virtual void danceWithMove(int move);
    virtual void dance();
    virtual void changeStateAccordingToSettings();
    virtual void vf324();
    virtual void vf328() {}
    virtual void deleteRest();
    virtual mVec3_c getPos();

    void loadRes();
    void updateAmiLine();
    void setZPos();
    bool isInQuicksand();
    void spawnQuicksandEffects();
    bool sub_80A73330(dActor_c *);
    void setBc();
    bool turnProc();
    float getWindMultiplier();
    void setMoveAnimation(char * name, m3d::playMode_e mode, float frame);
    void setBaseAnimation(char * name, m3d::playMode_e mode, float frame);
    bool sub_80A73BC0();
    void sub_80A73CB0();

    u8 mPad0[4];
    dHeapAllocator_c mNokoAllocator;
    nw4r::g3d::ResFile mNokoResFile;
    m3d::mdl_c mNokoModel;
    m3d::anmChr_c mMoveAnim;
    nw4r::g3d::ResAnmTexPat mNokoResAnmTexPat;
    m3d::anmTexPat_c mNokoAnimTex;
    u32 mIsFrozen;
    s16 m_8a8;
    bool mWalksOffLedges;
    mVec3_c m_8ac;
    float mXSpeedBeforeFrozen;
    u8 mPad3[8];
    float m_8c4;
    u32 m_8c8;
    int mDancesRemaining;
    mAng mYRotIncrease;
    u8 mPad4[2];
    u32 mDanceMove;
    mEf::levelOneEffect_c mQuickSandEffect;
    nodeCallback_c mMdlCallback;
};
