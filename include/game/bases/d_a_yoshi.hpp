#pragma once
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_player_model_manager.hpp>
#include <game/bases/d_yoshi_mdl.hpp>

class daYoshi_c : public daPlBase_c {
public:
    /// @unofficial
    enum AloneWaitSubstate_e {
        ALONE_WAIT_ACTION_0,
        ALONE_WAIT_ACTION_1,
        ALONE_WAIT_ACTION_2
    };

    /// @unofficial
    enum BlockOutSubstate_e {
        BLOCK_OUT_ACTION_0,
        BLOCK_OUT_ACTION_1,
        BLOCK_OUT_ACTION_2
    };

    /// @unofficial
    enum WalkSubstate_e {
        WALK_ANM_STATE_WAIT,
        WALK_ANM_STATE_MOVE
    };

    /// @unofficial
    enum CloudSubstate_e {
        CLOUD_ANM_STATE_INITIAL,
        CLOUD_ANM_STATE_MOVE,
        CLOUD_ANM_STATE_CROUCH,
        CLOUD_ANM_STATE_END_CROUCH
    };

    /// @unofficial
    enum EatAction_e {
        EAT_ACTION_START_TURN_WAIT,
        EAT_ACTION_START,
        EAT_ACTION_SUCCESS_EAT,
        EAT_ACTION_SUCCESS_DRINK,
        EAT_ACTION_SUCCESS_DRINK_BIG,
        EAT_ACTION_FAIL
    };

    daYoshi_c(); ///< @copydoc daPlBase_c::daPlBase_c()
    virtual ~daYoshi_c(); ///< @copydoc daPlBase_c::~daPlBase_c()

    virtual int create() override;
    virtual int doDelete() override;
    virtual int preExecute() override;
    virtual int draw() override;
    virtual s8 &getPlrNo() override;
    virtual bool isSpinLiftUpEnable() override;

    virtual void executeMain() override;
    virtual void executeLastPlayer() override;
    virtual void executeLastAll() override;
    virtual const sBcPointData *getHeadBgPointData() override { return &getBgPointData()->mHead; }
    virtual const sBcPointData *getWallBgPointData() override { return &getBgPointData()->mWall; }
    virtual const sBcPointData *getFootBgPointData() override { return &getBgPointData()->mFoot; }
    virtual float getStandHeadBgPointY() override;
    virtual void postBgCross() override;
    virtual float getSandSinkRate() override;
    virtual void setReductionScale() override;
    virtual void initStampReduction() override;
    virtual void calcJumpDaiReductionScale(int i1, int i2) override;
    virtual void setReductionBoyon() override;
    virtual bool setBalloonInDispOut(int) override;
    virtual bool isChange() override;
    virtual void changeNextScene(int) override;
    virtual bool isEnableDokanInStatus() override;
    virtual bool setHideNotGoalPlayer() override;
    virtual int setDemoGoal(mVec3_c &landPos, float goalCastleX, u8 goalType) override;
    virtual void setCreateAction(int) override;
    virtual bool setTimeOverDemo() override;
    virtual void setFallDownDemo() override;
    virtual bool updateDemoKimePose(ClearType_e clearType) override;
    virtual void initDemoGoalBase() override;
    virtual void executeDemoGoal_Run() override;
    virtual void changeState(const sStateIDIf_c &stateID, void *arg) override;

    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Walk);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Fall);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Land);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Crouch);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, SitJump);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Slip);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, HipAttack);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Funsui);
    STATE_VIRTUAL_FUNC_DECLARE(daYoshi_c, Cloud);

    bool checkWalkNextAction() override;
    virtual void setWaitActionAnm(AnmBlend_e blend) override;
    virtual void setWalkActionAnm(AnmBlend_e blend) override;
    virtual void walkActionInit_Wait(AnmBlend_e blend) override;
    virtual void walkAction_Wait() override;
    virtual void walkActionInit_Move(AnmBlend_e blend) override;
    virtual void walkAction_Move() override;
    virtual bool checkCrouch() override;
    virtual bool setCancelCrouch() override;
    virtual void slipActionMove(int) override;
    virtual void setFallAction() override;
    void releaseFunsuiAction() override;
    virtual float getCloudOffsetY() override;
    virtual const mVec3_c *getHeadTopPosP() override { return &getModel()->mHeadPos; }
    virtual int isStar() const override;
    virtual void setStar(StarSet_e, int) override;
    virtual void endStar() override;
    virtual void setVirusStar(daPlBase_c *) override;
    virtual void clearStarCount() override;
    virtual s8 getStarCount() const override;
    virtual s8 calcStarCount(int) override;
    virtual bool isNoDamage() override;
    virtual bool setDamage(dActor_c *, DamageType_e) override;
    virtual bool setForcedDamage(dActor_c *, DamageType_e) override;
    virtual bool setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) override;
    virtual bool _setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode) override;
    virtual bool setHipAttackOnEnemy(mVec3_c *attachPos) override;
    virtual void clearJumpActionInfo(int) override;
    virtual void setLandSE() override;
    virtual void set1UpKinokoEffect() override;
    virtual void setFlagGetEffect() override;
    virtual void setZPosition() override;
    virtual void setZPosition(float) override;
    virtual void setZPositionDirect(float) override;
    virtual void offZPosSetNone() override;
    virtual void startPlayerVoice(int, int) override;
    virtual void holdPlayerVoice(int, int) override;
    virtual short getMukiAngle(u8 direction) override;
    virtual int turnAngle() override;
    virtual bool setDamage2(dActor_c *, daPlBase_c::DamageType_e) override;

    // New main states
    STATE_FUNC_DECLARE(daYoshi_c, AloneWait);
    STATE_FUNC_DECLARE(daYoshi_c, BlockOut);
    STATE_FUNC_DECLARE(daYoshi_c, DamageRun);
    STATE_FUNC_DECLARE(daYoshi_c, StartRideOn);

    // Eat states
    STATE_FUNC_DECLARE(daYoshi_c, EatNone);
    STATE_FUNC_DECLARE(daYoshi_c, Eat);
    STATE_FUNC_DECLARE(daYoshi_c, EatOut);
    STATE_FUNC_DECLARE(daYoshi_c, EatMouth);

    void changeWalkAction();
    void setJumpCommonBase();
    void setJumpSpeed();
    void setJumpAnm();
    void setFunbariJumpEffect();
    void calcYoshiJump();
    void updateJumpAction();
    void setCrouchSmokeEffect();
    void CrouchAction_Ground();
    void setTurnSmokeEffect();
    void setTurnMoveSpeed();
    bool setHipAttackAction();
    void setCloudStateMove();
    void setCloudStateCrouch();
    dAcPy_c *getPlayerRideOn() const;
    bool checkRideOffAble();
    bool fn_8014eb70(dAcPy_c *player, int); ///< @unofficial
    void setRideOffPlayer();
    void changePlrNo(u8);
    void executeEatState();
    void changeEatState(const sStateIDIf_c &state);
    void setHitTongueActor(dActor_c *actor);
    bool fn_8014f030(dAcPy_c *player); ///< @unofficial
    bool setDamageSpitOut(bool);
    void finalizeEatCommon();
    bool calcOpenMouth();
    void eatDrinkBigCommonAction();
    bool releaseEatActor();
    void clearHitTongueReserve();
    bool checkHitTongueReserve(dCc_c *cc);
    void setHitTongueReserve();
    bool checkHitMouth(dActor_c *actor);
    void createYoshiEggCommon();
    bool createYoshiEgg();
    void addFruitCount();
    void selectAction();
    const sBcYoshiPointData *getBgPointData();
    bool getTongueTipMtx(mMtx_c *mtx);
    bool getMouthMtx(mMtx_c *mtx);
    void setTongueHitEffect(mVec3_c &pos);

    bool setEatAction();
    void setEatAction_StartTurnWait();
    void setEatAction_Start();
    void setEatAction_Success(int);
    void setEatAction_Fail();

    void EatAction_StartTurnWait();
    void EatAction_Start();
    void EatAction_SuccessEat();
    void EatAction_SuccessDrink();
    void EatAction_SuccessDrinkBig();
    void EatAction_Fail();

    void setDamageRunEffect();
    bool isEnableCreateEgg();
    void setCcAtYoshiMouthReq();
    void setCcAtYoshiEatReq();
    void setEatTongueCall(dActor_c *actor);
    void setEatTongueOffCall(dActor_c *actor);
    void setEatMouthCall(dActor_c *actor);
    bool setEatSpitOutCall(dActor_c *actor);
    bool setEatGlupDownCall(dActor_c *actor);

    void setEatOutSE();
    void setEatActorMouthIn();
    void checkYoshiEggCommon();
    void setCcData();
    void setBcData();
    void setCcAtYoshiEat();
    void setCcAtYoshiMouth();

    void changeState(const sStateIDIf_c &stateID) {
        changeState(stateID, 0);
    }

    template <typename T>
    void changeState(const sStateIDIf_c &stateID, T arg) {
        changeState(stateID, (void *) arg);
    }

    bool isEatState(sStateIDIf_c &id) {
        return mEatStateMgr.getStateID()->isEqual(id);
    }

    dYoshiMdl_c *getModel() { return (dYoshiMdl_c *) daPlBase_c::getModel(); }
    int getFruitCount() const { return mFruitCount; }

    static bool ccCheckAttack(dCc_c *self, dCc_c *other);
    static bool ccCheckStamp(dCc_c *self, dCc_c *other);
    static void ccCallBack(dCc_c *self, dCc_c *other);
    static void atCcCallBack(dCc_c *self, dCc_c *other);

    dPyMdlMng_c mModelMng;
    sFStateMgr_c<daYoshi_c, sStateMethodUsr_FI_c> mEatStateMgr;
    u32 mEatAction;
    fBaseID_e m_4c;
    fBaseID_e mPlayerRideOn;
    float m_54;
    u8 m_58;
    bool m_59;
    int m_5c;
    int m_60;
    int m_64;
    int m_68;
    u8 mYoshiDirection;
    s8 mNum;
    int m_70;
    int mSomeTimer;
    int m_78;
    int m_7c;
    float m_80;
    dCc_c *m_84;
    float m_88;
    int m_8c;
    int mFruitCount;
    int m_94;
    mEf::levelEffect_c mLevelEffect1;
    mEf::levelEffect_c mLevelEffect2;
    int m_2e8;
    u8 mPad[0x4];

    ACTOR_PARAM_CONFIG(Color, 0, 4);
    ACTOR_PARAM_CONFIG(BeginType, 16, 8);
};
