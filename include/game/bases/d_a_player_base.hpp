#pragma once

#include <constants/game_constants.h>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_ac_py_key.hpp>
#include <game/bases/d_quake.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/sLib/s_State.hpp>

class daPlModel_c {
public:
    u8 pad[0x108];
    mMtx_c mMtx;

    mMtx_c &getMtx() { return mMtx; }
};

class daPlBase_c : public dActor_c {
public:
    enum DamageType_e {
        DAMAGE_NONE = 0,
        DAMAGE_1, DAMAGE_2, DAMAGE_3, DAMAGE_4,
        DAMAGE_5, DAMAGE_6, DAMAGE_7, DAMAGE_8,
        DAMAGE_9, DAMAGE_A, DAMAGE_B, DAMAGE_C
    };

    enum DokanDir_e {
        DOKAN_U,
        DOKAN_D,
        DOKAN_L,
        DOKAN_R,
        DOKAN_ROLL
    };

    enum StarSet_e {};

    enum AnmBlend_e {
        BLEND_0,
        BLEND_1
    };

    daPlBase_c();
    virtual ~daPlBase_c();

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual int doDelete();
    virtual int preDraw();
    virtual int draw();

    virtual void executeMain() {}
    virtual void executeLastPlayer() {}
    virtual void executeLastAll() {}
    virtual bool isItemKinopio() { return false; }
    virtual void setPowerup(PLAYER_POWERUP_e, int); ///< @unofficial
    virtual u8 getTallType(s8);
    virtual void *getHeadBgPointData() { return nullptr; };
    virtual void *getWallBgPointData() { return nullptr; };
    virtual void *getFootBgPointData() { return nullptr; };
    virtual float getStandHeadBgPointY() { return 0.0f; }
    virtual void checkBgCrossSub() {}
    virtual void postBgCross();
    virtual float getSandSinkRate() { return 0.0f; }
    virtual void setReductionScale();
    virtual void initStampReduction();
    virtual void calcJumpDaiReductionScale(int, int);
    virtual void setReductionBoyon();
    virtual bool setPressBgDamage(int, int);
    virtual bool setBalloonInDispOut(int) { return false; }
    virtual bool isChange() { return false; }
    virtual void changeNextScene(int);
    virtual bool isEnableDokanInStatus(void);
    virtual bool setHideNotGoalPlayer();
    virtual int vf130(float f, mVec2_c *v, int param3); ///< @unofficial
    virtual bool setDemoCannonWarp(int, short, short) { return false; }

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNone);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoStartWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanRoll);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoRailDokan);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoDown);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNextGotoBlock);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoGoal);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoControl);

    virtual void initialDokanUnder();
    virtual void initialDokanUper();
    virtual void initialDokanRight();
    virtual void initialDokanLeft();
    virtual void initialDokanUnderM();
    virtual void initialDokanUperM();
    virtual void initialDokanRightM();
    virtual void initialDokanLeftM();
    virtual void initialDokanDepth();
    virtual void initialDoor();
    virtual void initialFall();
    virtual void initialVine();
    virtual void initialJumpRight();
    virtual void initialJumpLeft();
    virtual void initialHipAttack();
    virtual void initialSlip();
    virtual void initialSwim();
    virtual void initialBlockJump();
    virtual void initialBlockJumpBelow();
    virtual void initialTorideBoss();
    virtual void initialNormal();

    virtual void setCreateAction(int);
    virtual bool setTimeOverDemo() { return false; }
    virtual void setFallDownDemo() {}
    virtual bool setDokanIn(DokanDir_e dir);
    virtual void initDemoOutDokan();
    virtual bool vf284(int); ///< @unofficial
    virtual void initDemoGoalBase();
    virtual void executeDemoGoal_Run();
    virtual void initializeDemoControl() {}

    virtual void changeState(const sStateIDIf_c &, void *);

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, None);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Walk);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, SitJump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Fall);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Land);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Crouch);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Slip);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, HipAttack);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Swim);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, JumpDai);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, PlayerJumpDai);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Funsui);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Kani);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Cloud);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, AnimePlay);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, WaitJump);

    virtual bool isWaitFrameCountMax() { return false; }
    virtual bool checkWalkNextAction() { return false; }
    virtual void setWaitActionAnm(AnmBlend_e);
    virtual void setWalkActionAnm(AnmBlend_e);
    virtual void walkActionInit_Wait(AnmBlend_e);
    virtual void walkAction_Wait();
    virtual void walkActionInit_Move(AnmBlend_e);
    virtual void walkAction_Move();
    virtual bool checkCrouch();
    virtual bool setCancelCrouch();
    virtual void setSlipAction();
    virtual void slipActionMove(int);
    virtual void setFallAction();
    virtual void setHipAttack_AttackStart();

    virtual void releaseFunsuiAction();

    virtual float getCloudOffsetY();
    virtual bool setRideJrCrown(const dActor_c *) { return false; }
    virtual bool isRideJrCrownOwn(const dActor_c *) { return false; }
    virtual void setRideJrCrownMtx(const mMtx_c *) {}
    virtual void setRideJrCrownAnm(int) {}

    virtual float *getHeadTopPosP() { return nullptr; }
    virtual float *getGravityData(); // { return mGravityData; }
    virtual bool isCarry() const { return false; }
    virtual bool isLiftUp() { return false; }

    virtual bool isLiftUpExceptMame() { return false; }
    virtual int isStar() const;
    virtual void setStar(StarSet_e, int);
    virtual void endStar() {}
    virtual void setVirusStar(daPlBase_c *) {}
    virtual void clearStarCount();
    virtual s8 getStarCount() const; // { return mStarCount; }
    virtual s8 calcStarCount(int);

    virtual bool isNoDamage();
    virtual bool setDamage(dActor_c *, DamageType_e);
    virtual bool setForcedDamage(dActor_c *, DamageType_e);

    virtual u32 vf3fc(float, float, int, int, int); ///< @unofficial
    virtual u32 vf400(float, float, int, int, int); ///< @unofficial
    virtual bool setWaitJump(float);

    virtual bool setHipAttackOnEnemy(mVec3_c *);

    virtual void clearJumpActionInfo(int) {}
    virtual bool setSwimSpeed(float, float);
    virtual void setLandSE();

    virtual void set1UpKinokoEffect() {}
    virtual void setFlagGetEffect() {}
    virtual void setHipAttackEffect();

    virtual void setZPosition();
    virtual void setZPosition(float);
    virtual void setZPositionDirect(float);
    virtual void offZPosSetNone();

    virtual void vf434(int, int); ///< @unofficial
    virtual void vf438(int, int); ///< @unofficial

    virtual void startQuakeShock(dQuake_c::TYPE_SHOCK_e);
    virtual void startPatternRumble(const char *pattern);

    virtual short getMukiAngle(u8 direction);
    virtual int turnAngle();
    virtual void maxFallSpeedSet();

    virtual bool setDamage2(dActor_c *, daPlBase_c::DamageType_e);

    bool isMameAction();

    mVec3_c getAnkleCenterPos();

    bool isStatus(int key);

    float getAnkleCenterX() { return getAnkleCenterPos().x; }
    float getAnkleCenterY() { return getAnkleCenterPos().y; }

    int getTreadCount() { return mTreadCount; }

    daPlModel_c *getModel();

    char mPad1[0xc9e];
    float mSmallScoreOffset;
    char mPad2[0x1];
    u8 mAmiLayer;
    char mPad3[0x4a];
    s8 mTreadCount;
    s8 m_ced;
    s8 m_cee;
    char mPad4[0x52];
    u32 mFlags;
    u8 mPad5[0x3b4];
};
