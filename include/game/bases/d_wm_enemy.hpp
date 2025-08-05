#pragma once

#include <game/bases/d_wm_demo_actor.hpp>
#include <game/bases/d_wm_en_path.hpp>
#include <game/bases/d_wm_bg_sync.hpp>

/// @ingroup bases
class dWmEnemy_c : public dWmDemoActor_c {
public:
    /// @unofficial
    struct JumpData_s {
        int mFrames;
        float mJumpSpeed;
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
    virtual mVec3_c getPointOffset(int);
    virtual short getWaitAngle() { return 0; }
    virtual float getWalkAnmRate() { return 1.0f; }
    virtual void setWalkAnm(float) {}
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
    virtual int isWaitWalkEnd() { return m_6c4 != 4; }

    void initializeBase(const char **, int, bool);
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
    void ModelCalc(m3d::mdl_c *mdl, float f1, float f2, float f3);
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
    dWmEnPath_c mPath;
    mVec3_c m_698;
    mVec3_c m_6a4;
    u8 mPad1[2];
    short m_6b2;
    u8 mPad2[2];
    bool m_6b6;
    bool m_6b7;
    int m_6b8;
    int m_6bc;
    int m_6c0;
    int m_6c4;
    int m_6c8;
    int m_6cc;
    dWmBgmSync_c *mpBgmSync;
    int m_6d4;
    bool m_6d8;
    int m_6dc;
    int m_6e0;
    bool m_6e4;
    bool mEnWalk;

public:
    static const char *smc_PathPointA[];
    static const char *smc_PathPointB[];
    static const char *smc_PathPointC[];
};
