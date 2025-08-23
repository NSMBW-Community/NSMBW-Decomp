#pragma once

#include <game/bases/d_wm_demo_actor.hpp>
#include <game/bases/d_wm_en_path.hpp>
#include <game/bases/d_wm_bgm_sync.hpp>

/// @ingroup bases
/// @paramtable
class dWmEnemy_c : public dWmDemoActor_c {
public:
    /// @unofficial
    struct JumpData_s {
        int m_00;
        float mJumpSpeed;
        short mFrames;
        float mStartScale;
        float mEndScale;
    };

    dWmEnemy_c() {}
    ~dWmEnemy_c();

    virtual int create() { return SUCCEEDED; }
    virtual int execute();
    virtual int draw() { return SUCCEEDED; }
    virtual int doDelete() { return SUCCEEDED; }
    virtual int GetActorType() { return ACTOR_MAP_ENEMY; }

    virtual int GetIndex() { return isDead() ? -1 : mPath.mpCurrentPoint->mPointIndex; }
    virtual int GetNextIndex() { return mPath.GetNextPointIdx(); }
    virtual float GetTerritory() { return 0.0f; }
    virtual void initWalk() {}
    virtual bool doWalk();
    virtual void initDemoAnger();
    virtual bool procDemoAnger();
    virtual void initDemoLose();
    virtual bool procDemoLose();
    virtual void procDemoLoseEnd();
    virtual void initDemoStarLose();
    virtual bool procDemoStarLose();
    virtual void initDemoBgmDance();
    virtual bool procDemoBgmDance();
    virtual mVec3_c calcBlowOffPos(float offsetX) { return mVec3_c(mPos.x + offsetX, mPos.y, mPos.z); }
    virtual mVec3_c getPointOffset(int index);
    virtual short getWaitAngle() { return 0; }
    virtual float getWalkAnmRate() { return 1.0f; }
    virtual void setWalkAnm(float anmRate) {}
    virtual bool IsHitToWaitPlayer();
    virtual int getStartPoint();
    virtual int vfc4(int pointNo) { return pointNo; } ///< @unofficial
    virtual void calc();
    virtual void calculateEffect();
    virtual int getEnemyWalkSeID();
    virtual bool IsPlayerComing() { return IsPlayerComingCore(); }
    virtual bool IsRandomWalk();
    virtual bool IsRandomMove() { return true; }
    virtual short GetChangeDirRate() { return 50; }
    virtual int GetMoveRate() { return 100; }
    virtual short GetWalkWaitFrame() { return 20; }
    virtual void PostWaitWalk() {}
    virtual void updateBgmAnimRate();
    virtual int isWaitWalkEnd() { return mCurrProc != PROC_TYPE_WAIT_WALK; }

    void initializeBase(const char **names, int count, bool cyclic);
    void initShapeAngle();
    bool IsExecEnable();
    bool IsNeedChasePlayer();
    mVec3_c getNextPointInfo();
    mVec3_c getCurrentPointInfo();
    void initWalk(float);
    bool isNextThroughPoint();
    bool CheckIsHitToPlayer();
    void updatePathInfo(bool);
    void deleteSound();
    void deleteEffect();
    bool csCommand(int id, bool b); ///< @unofficial
    bool FUN_800f88d0(); ///< @unofficial (`isMainCast`?)
    void demoJump(const mVec3_c &pos, JumpData_s &jumpData, float scale); ///< @unofficial
    bool isThroughPoint(int);
    bool isDead();
    void ModelCalc(m3d::mdl_c *mdl, float yOffset, float shadowYOffset, float shadowScale);
    bool IsPlayerComingCore();
    bool IsHitToMovePlayer();
    bool isAllEnemyMoveEnd();
    bool procDemoLoseBase(short angle);

    void init_exec();
    void mode_exec();
    void init_DemoContinue();
    void mode_DemoContinue();
    void init_bgmDance();
    void mode_bgmDance();
    void init_lose();
    void mode_lose();
    void init_waitWalk();
    void mode_waitWalk();

    typedef void (dWmEnemy_c::*ProcFunc)();

private:
    struct unk_s {
        float m_00, m_04, m_08, m_0c;
        u8 mPad[0x42];
        mAng3_c mAngle1;
        mAng3_c mAngle2;
    };

    enum PROC_TYPE_e {
        PROC_TYPE_EXEC,
        PROC_TYPE_DEMO_CONTINUE,
        PROC_TYPE_BGM_DANCE,
        PROC_TYPE_LOSE,
        PROC_TYPE_WAIT_WALK,
        PROC_COUNT
    };

    enum POINT_TYPE_e {
        POINT_TYPE_STOP,
        POINT_TYPE_THROUGH
    };

    dWmEnPath_c mPath;
    mVec3_c mNextPoint;
    mVec3_c mPrevPoint;
    mAng3_c mDemoAngle;
    bool mArrivedAtTarget;
    bool mEnWalk;
    POINT_TYPE_e mNextPointType; ///< 0 = stop point, 1 = through point
    int mRotateTimer;
    unk_s *mpUnkData;
    PROC_TYPE_e mCurrProc;
    int mWalkWaitTimer;
    int mDemoContinueRelated;
    dWmBgmSync_c *mpBgmSync;
    int mBgmDanceRelated;
    bool mNotAnger;
    int mSeID;
    int mEfID;
    bool mHitPlayer;

    ACTOR_PARAM_CONFIG(enemyNo, 0, 4);
    ACTOR_PARAM_CONFIG(startPoint, 4, 4);

public:
    static const char *smc_PathPointA[];
    static const char *smc_PathPointB[];
    static const char *smc_PathPointC[];
};
