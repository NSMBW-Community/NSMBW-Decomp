#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_rc.hpp>

class dAcPy_c;

class dActor_c : public dBaseActor_c {
public:
    enum YOSHI_POINTS_e {
        YOSHI_POINTS_200,
        YOSHI_POINTS_1000,
        YOSHI_POINTS_NONE
    };

    dActor_c();
    ~dActor_c();

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual const char *getKindString() const;

    virtual bool ActorDrawCullCheck();
    virtual void block_hit_init();
    virtual bool vf68() { return true; }
    virtual s8 *getPlrNo() { return &mPlayerNo; }
    virtual mVec2_c getLookatPos() const;
    virtual bool isSpinLiftUpEnable() { return true; }
    virtual void setSpinLiftUpActor(dActor_c *actor);
    virtual void setCarryFall(dActor_c *actor, int) {}
    virtual void setEatTongue(dActor_c *actor);
    virtual void setEatTongueOff(dActor_c *actor);
    virtual void setEatMouth(dActor_c *actor);
    virtual bool setEatSpitOut(dActor_c *actor);
    virtual bool setEatGlupDown(dActor_c *actor);
    virtual void eatMove(dActor_c *actor);
    virtual void removeCc();
    virtual void reviveCc();
    virtual void setAfterEatScale();
    virtual void calcSpitOutPos(dActor_c *actor);
    virtual float calcEatScaleRate(dActor_c *actor);
    virtual void calcEatInScale(dActor_c *actor);
    virtual void allEnemyDeathEffSet();
    virtual void vfb4();
    virtual void funsuiMoveX() {}
    virtual void cancelFunsuiActUpper();
    virtual void cancelFunsuiActSide(int);
    virtual void cancelFunsuiActVanish();
    virtual void waterSplashEffect(const mVec3_c &pos, float size);
    virtual void yoganSplashEffect(const mVec3_c &pos, float size);
    virtual void poisonSplashEffect(const mVec3_c &pos, float size);

    bool checkAreaNo();
    static void setSoftLight_Player(m3d::bmdl_c &mdl);
    static void setSoftLight_Enemy(m3d::bmdl_c &mdl);
    static void setSoftLight_Map(m3d::bmdl_c &mdl);
    static void setSoftLight_MapObj(m3d::bmdl_c &mdl);
    static void setSoftLight_Boss(m3d::bmdl_c &mdl);
    static void setSoftLight_Item(m3d::bmdl_c &mdl);
    void deleteActor(u8);

    /// @unofficial
    bool cullCheck(const mVec3_c &pos, const mBoundBox &bound, u8 areaID) const;

    bool ActorScrOutCheck(u16 flags);

    static bool otherCullCheck(const mVec3_c &pos, const mBoundBox &bound, mBoundBox bound2, u8 areaID);

    bool checkBgColl(); ///< @unofficial

    bool carryFukidashiCheck(int otherPlayerNo, mVec2_c triggerSize);

    void touchFlagpole(s8 scoreSetType, int noScore);

    dAcPy_c *searchCarryFukidashiPlayer(int carryPlayerNo);

    void carryFukidashiCancel(int carriedPlayerNo, int carryingPlayerNo);

    void slideComboSE(int count, bool clapType);
    void clrComboCnt();

    bool checkCarried(int *res);

    static void setSearchNearPlayerFunc(int loopType);
    static void setGetTrgToSrcDirFunc(int loopType);
    static void setChangePosAngleFunc(int loopType);
    static void setLoopFunc(int loopType);

    dAcPy_c *searchNearPlayer(mVec2_c &pos);
    static dAcPy_c *searchNearPlayer_Main(mVec2_c &delta, const mVec2_c &selfPos);
    static dAcPy_c *searchNearPlayerNormal(mVec2_c &delta, const mVec2_c &selfPos);
    static dAcPy_c *searchNearPlayerLoop(mVec2_c &delta, const mVec2_c &selfPos);

    static bool getTrgToSrcDir_Main(float x1, float x2);
    static bool getTrgToSrcDirNormal(float x1, float x2);
    static bool getTrgToSrcDirLoop(float x1, float x2);

    static void changePosAngle(mVec3_c *pos, mAng3_c *ang, int param3);

    void setKind(u8 kind);

    static void setTmpCtData(u8 layer);

    static dActor_c *construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer);
    static dActor_c *construct(ProfileName profName, dBase_c *parent, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer);

    typedef dAcPy_c *(*searchNearPlayerFunc)(mVec2_c &, const mVec2_c &);
    typedef bool (*getTrgToSrcDirFunc)(float, float);

    u8 m_00;
    u32 mCarryingPlayerNo;
    u32 mCarryingFlags;
    u32 mCarryingDirection;
    u32 mComboCount;
    u8 m_13;
    u32 m_17;
    float m_1b;
    u32 m_1f;

    dCc_c mCc;
    dBc_c mBc;
    dRc_c mRc;

    mVec2_c m_1eb;

    mVec2_c mVisibleAreaSize;
    mVec2_c mVisibleAreaOffset;
    mBoundBox mMaxBound;
    mBoundBox mDestroyBound;
    bool mDirection;
    u8 mAreaNo;
    u8 mBgCollFlags; // &1 = head, &2 = wallL, &4 = wallR, &8 = foot

    u8 *mpDeleteFlags;
    u16 *mpDeleteVal;
    u32 m_22f;
    u64 m_233;

    u32 m_23b;
    u16 m_23f;
    bool mBlockHit;

    u32 mEatenByID;
    u8 mEatState; // 0 = not eaten, 2 = eaten, 4 = spat out
    u8 mEatSpitType;
    mVec3_c mPreEatScale;

    YOSHI_POINTS_e mYoshiEatPoints;
    u32 m_25b;
    u32 m_25f;

    void *mCarryPropelActor;
    u8 mKind;
    s8 mPlayerNo;
    u8 mStopMask;
    u8 mLayer;
    bool mNoRespawn;
    bool m_26c;

    mBoundBox getDestroyBound() { return mDestroyBound; }

    void setDefaultMaxBound() {
        mMaxBound.set(smc_CULL_XLIMIT, smc_CULL_YLIMIT, smc_CULL_AREA_XLIMIT, smc_CULL_AREA_YLIMIT);
    }

    float getCenterX() { return getCenterPos().x; }
    float getCenterY() { return getCenterPos().y; }

    u8 getKindMask() { return 1 << mKind; }

    static const float smc_CULL_XLIMIT;
    static const float smc_CULL_YLIMIT;
    static const float smc_CULL_AREA_XLIMIT;
    static const float smc_CULL_AREA_YLIMIT;
    static const mVec2_c smc_FUKIDASHI_RANGE;

    static bool mExecStopReq;
    static bool mDrawStopReq;
    static bool mExecStop;
    static bool mDrawStop;
    static searchNearPlayerFunc mSearchNearPlayerFunc;
    static getTrgToSrcDirFunc mGetTrgToSrcDirFunc;
    static u8 m_tmpCtLayerNo;

    static u32 m_relatedCreate1; ///< @unofficial
    static u64 m_relatedCreate2; ///< @unofficial
    static u32 m_relatedCreate3; ///< @unofficial
    static u32 m_relatedCreate4; ///< @unofficial
    static u8 m_relatedCreate5; ///< @unofficial
};

extern const u8 l_Ami_Line[8];
extern const float l_Ami_Zpos[2];
