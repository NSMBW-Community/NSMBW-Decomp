#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_rc.hpp>

class dAcPy_c;

class dActor_c : public dBaseActor_c {
public:
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
    virtual void setSpinLiftUpActor(dActor_c *);
    virtual void setCarryFall(dActor_c*, int) {}
    virtual void setEatTongue(dActor_c *);
    virtual void setEatTongueOff(dActor_c *);
    virtual void setEatMouth(dActor_c *);
    virtual bool setEatSpitOut(dActor_c *);
    virtual bool setEatGlupDown(dActor_c *);
    virtual void eatMove(dActor_c *);
    virtual void removeCc();
    virtual void reviveCc();
    virtual void setAfterEatScale();
    virtual void calcSpitOutPos(dActor_c *);
    virtual float calcEatScaleRate(dActor_c *);
    virtual void calcEatInScale(dActor_c *);
    virtual void allEnemyDeathEffSet();
    virtual void vfb4();
    virtual void funsuiMoveX() {}
    virtual void cancelFunsuiActUpper();
    virtual void cancelFunsuiActSide(int);
    virtual void cancelFunsuiActVanish();
    virtual void waterSplashEffect(const mVec3_c &, float);
    virtual void yoganSplashEffect(const mVec3_c &, float);
    virtual void poisonSplashEffect(const mVec3_c &, float);

    bool checkAreaNo();
    static void setSoftLight_Player(m3d::bmdl_c &);
    static void setSoftLight_Enemy(m3d::bmdl_c &);
    static void setSoftLight_Map(m3d::bmdl_c &);
    static void setSoftLight_MapObj(m3d::bmdl_c &);
    static void setSoftLight_Boss(m3d::bmdl_c &);
    static void setSoftLight_Item(m3d::bmdl_c &);
    void deleteActor(u8);

    /// @unofficial
    bool cullCheck(const mVec3_c &pos, const mBoundBox &bound, u8 areaID) const;

    bool ActorScrOutCheck(u16);

    static bool otherCullCheck(const mVec3_c &pos, const mBoundBox &bound, mBoundBox bound2, u8 areaID);

    // @unofficial
    bool checkBgColl();

    bool carryFukidashiCheck(int param_2, mVec2_c param_3);

    void touchFlagpole(s8, int);

    dAcPy_c *searchCarryFukidashiPlayer(int);

    void carryFukidashiCancel(int idk, int plNo);

    void slideComboSE(int param_1, bool param_2);
    void clrComboCnt();

    bool checkCarried(int *);

    static void setSearchNearPlayerFunc(int);
    dAcPy_c *searchNearPlayer(mVec2_c &);
    static dAcPy_c *searchNearPlayer_Main(mVec2_c &, const mVec2_c &);

    static void setGetTrgToSrcDirFunc(int);

    static dAcPy_c *searchNearPlayerNormal(mVec2_c &, const mVec2_c &);
    static dAcPy_c *searchNearPlayerLoop(mVec2_c &, const mVec2_c &);

    static bool getTrgToSrcDirNormal(float, float);
    static bool getTrgToSrcDirLoop(float, float);

    static bool getTrgToSrcDir_Main(float, float);

    static void changePosAngle(mVec3_c *pos, mAng3_c *ang, int unk);
    static void setChangePosAngleFunc(int);

    static void setLoopFunc(int loopType);

    void setKind(u8 kind);

    static void setTmpCtData(u8 layer);

    static dActor_c *construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer);
    static dActor_c *construct(ProfileName profName, dBase_c *parent, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer);

    typedef dAcPy_c *(*searchNearPlayerFunc)(mVec2_c &, const mVec2_c &);
    typedef bool (*getTrgToSrcDirFunc)(float, float);

    u8 _0;
    u32 mCarryingPlayerNo;
    u32 carryingFlags;
    u32 carryingDirection;
    u32 mComboCount;
    u8 _10;
    u32 _14;
    float _18;
    u32 _1c;

    dCc_c mCc;
    dBc_c mBc;
    dRc_c mRc;

    float _1e8;
    float _1ec;

    // TODO make these bounding boxes
    mVec2_c visibleAreaSize;
    mVec2_c visibleAreaOffset;
    mVec2_c mMaxBound1;
    mVec2_c mMaxBound2;
    mBoundBox mDestroyBound;
    bool isFacingLeft;
    u8 mAreaNo;
    u8 mBgCollFlags;

    u8* _224;
    u16* _228;
    u32 _22c;
    u8 _230[2];

    u32 _234;

    u32 _238;
    u16 _23c;
    u8 _23e;

    u32 eaterActorID;
    u8 mNoDrawIf2;
    u8 _245;
    mVec3_c mPreEatScale;

    int _254;
    u32 _258;
    u32 _25c;

    void* carryPropelActor;
    u8 mKind;
    s8 mPlayerNo;
    u8 mStopMask;
    u8 mLayer;
    bool mWasSquished;
    bool mUnkBool; // enemy related

    mBoundBox getDestroyBound() { return mDestroyBound; }

    static float getDefaultX() { return 80; }
    static float getDefaultY() { return 256; }

    void setDefaultMaxBound() {
        mMaxBound1.set(smc_CULL_XLIMIT, smc_CULL_YLIMIT);
        mMaxBound2.set(smc_CULL_AREA_XLIMIT, smc_CULL_AREA_YLIMIT);
    }

    float getCenterX() { return getCenterPos().x; }
    float getCenterY() { return getCenterPos().y; }

    static const float smc_CULL_XLIMIT;
    static const float smc_CULL_YLIMIT;
    static const float smc_CULL_AREA_XLIMIT;
    static const float smc_CULL_AREA_YLIMIT;
    static mBoundBox bb;
    static const mVec2_c smc_FUKIDASHI_RANGE;

    static bool mExecStopReq;
    static bool mDrawStopReq;
    static bool mExecStop;
    static bool mDrawStop;
    static searchNearPlayerFunc mSearchNearPlayerFunc;
    static getTrgToSrcDirFunc mGetTrgToSrcDirFunc;
    static u8 m_tmpCtLayerNo;

    static u32 m_relatedCreate1; ///< @unofficial
    static u8 m_relatedCreate2[8]; ///< @unofficial
    static u32 m_relatedCreate3; ///< @unofficial
    static u32 m_relatedCreate4; ///< @unofficial
    static u8 m_relatedCreate5; ///< @unofficial

    u8 getKindMask() { return 1 << mKind; }
};

extern const u8 l_Ami_Line[8];
extern const float l_Ami_Zpos[2];
