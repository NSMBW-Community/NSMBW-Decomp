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
    bool setHipAttackOnEnemy(mVec3_c *attachPos) override;
    virtual short getMukiAngle(u8 direction) override;
    virtual int turnAngle() override;

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
    void setEatTongueOffCall(dActor_c *actor);
    bool calcOpenMouth();
    void eatDrinkBigCommonAction();

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
    bool setEatTongueCall(dActor_c *actor);
    bool setEatSpitOutCall(dActor_c *actor);
    bool setEatMouthCall(dActor_c *actor);
    bool setEatGlupDownCall(dActor_c *actor);

    void setEatOutSE();
    void releaseEatActor();
    void setEatActorMouthIn();
    void checkYoshiEggCommon();

    bool getTongueTipMtx(mMtx_c *mtx);
    void getMouthMtx(mMtx_c *mtx);

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

    static void ccCallback(dCc_c *self, dCc_c *other);
    static void atCcCallback(dCc_c *self, dCc_c *other);

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
    u8 mPad1[0x4];
    int mSomeTimer;
    int m_78;
    int m_7c;
    float m_80;
    u8 mPad2[0xc];
    int mFruitCount;
    u8 mPad3[0x4];
    mEf::levelEffect_c mLevelEffect1;
    mEf::levelEffect_c mLevelEffect2;
    int m_2e8;
    u8 mPad4[0x4];

    ACTOR_PARAM_CONFIG(BeginType, 16, 8);
};
