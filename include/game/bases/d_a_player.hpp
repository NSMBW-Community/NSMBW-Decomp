#pragma once
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_hio.hpp>
#include <game/bases/d_player_orchestra.hpp>
#include <game/bases/d_propel_parts.hpp>
#include <game/bases/d_pc.hpp>

class daYoshi_c;

/**
 * @brief The player class for Mario, Luigi and the Toads.
 * @statetable
 */
class dAcPy_c : public daPlBase_c {
public:
    /// @unofficial
    enum SpinHipAttackSubstate_e {
        SPIN_HIP_ATTACK_ACTION_0,
        SPIN_HIP_ATTACK_ACTION_1
    };

    /// @unofficial
    enum FallSubstate_e {
        FALL_ACTION_0,
        FALL_ACTION_1
    };

    /// @unofficial
    enum JumpSubstate_e {
        JUMP_TAKE_OFF,
        JUMP_AIR
    };

    /// @unofficial
    enum LandSubstate_e {
        LAND_ACTION_0,
        LAND_ACTION_1
    };

    /// @unofficial
    enum SpinJumpSubstate_e {
        SPIN_JUMP_ACTION_0,
        SPIN_JUMP_ACTION_1
    };

    /// @unofficial
    enum SitJumpSubstate_e {
        SIT_JUMP_ACTION_0,
        SIT_JUMP_ACTION_1
    };

    /// @unofficial
    enum CannonJumpSubstate_e {
        CANNON_JUMP_ACTION_0,
        CANNON_JUMP_ACTION_1,
        CANNON_JUMP_ACTION_2
    };

    /// @unofficial
    enum BlockJumpSubstate_e {
        BLOCK_JUMP_ACTION_0,
        BLOCK_JUMP_ACTION_1
    };

    /// @unofficial
    enum RollSlipSubstate_e {
        ROLL_SLIP_ACTION_0,
        ROLL_SLIP_ACTION_1,
        ROLL_SLIP_ACTION_2
    };

    /// @unofficial
    enum PenguinSlideSubstate_e {
        PENGUIN_SLIDE_ACTION_0,
        PENGUIN_SLIDE_ACTION_1,
        PENGUIN_SLIDE_ACTION_2
    };

    /// @unofficial
    enum CrouchSubstate_e {
        CROUCH_GROUND,
        CROUCH_WATER
    };

    /// @unofficial
    enum ThrowSubstate_e {
        THROW_ACTION_0,
        THROW_ACTION_1,
        THROW_ACTION_2
    };

    /// @unofficial
    enum SwimSubstate_e {
        SWIM_ACTION_0,
        SWIM_ACTION_1,
        SWIM_ACTION_2,
        SWIM_ACTION_3
    };

    /// @unofficial
    enum VineSubstate_e {
        VINE_ACTION_IVY,
        VINE_ACTION_NET,
        VINE_ACTION_ATTACK,
        VINE_ACTION_ROLL
    };

    /// @unofficial
    enum HangSubstate_e {
        HANG_ACTION_START,
        HANG_ACTION_WAIT,
        HANG_ACTION_MOVE
    };

    /// @unofficial
    enum KaniSubstate_e {
        KANI_ACTION_WALK,
        KANI_ACTION_HANG_INIT,
        KANI_ACTION_JUMP_HANG_INIT,
        KANI_ACTION_HANG,
        KANI_ACTION_HANG_FALL,
        KANI_ACTION_HANG_UP,
        KANI_ACTION_HANG_UP_VINE
    };

    /// @unofficial
    enum RopeSwingState_e {
        ROPE_SWING_0,
        ROPE_SWING_1,
        ROPE_SWING_2,
        ROPE_SWING_3,
        ROPE_SWING_4,
        ROPE_SWING_5,
        ROPE_SWING_6,
        ROPE_SWING_7,
        ROPE_SWING_8,
        ROPE_SWING_9,
        ROPE_SWING_10
    };

    /// @unofficial
    enum DemoOutDoorState_e {
        DEMO_OUT_DOOR_OPEN_DOOR,
        DEMO_OUT_DOOR_MOVE_CENTER,
        DEMO_OUT_DOOR_MOVE_INTER,
        DEMO_OUT_DOOR_WAIT_CLOSE,
        DEMO_OUT_DOOR_WAIT_ENTER,
        DEMO_OUT_DOOR_FINISHED
    };

    enum QuakeMode_e {
        QUAKE_MODE_0
    };

    struct GlobalData_t {
        float f1, f2, mSandSinkRate, f4, f5, f6, f7, f8;
        mVec3_c mPos[3];
        float mThrowSpeed1;
        float mThrowSpeed2;
        float mThrowSpeedMax;
        float mSpitRelated[4];
        float mVineSpeedRelated[4];
        float mKaniSpeedRelated[4];
    };

    dAcPy_c();
    virtual ~dAcPy_c();

    virtual int create();
    virtual int preExecute();
    virtual void postExecute(MAIN_STATE_e status);
    virtual int doDelete();
    virtual int draw();

    virtual bool isSpinLiftUpEnable();
    virtual void setSpinLiftUpActor(dActor_c *carryingActor);

    virtual void setEatTongue(dActor_c *eatingActor);
    virtual void setEatTongueOff(dActor_c *eatingActor);
    virtual void setEatMouth(dActor_c *eatingActor);
    virtual bool setEatSpitOut(dActor_c *eatingActor);
    virtual void eatMove(dActor_c *eatingActor);

    virtual void executeMain();
    virtual void executeLastPlayer();
    virtual void executeLastAll();
    virtual bool isItemKinopio();
    virtual void setPowerup(PLAYER_POWERUP_e, int);
    virtual u8 getTallType(s8);
    virtual const sBcPointData *getHeadBgPointData() { return &getBgPointData()->mHead; }
    virtual const sBcPointData *getWallBgPointData() { return &getBgPointData()->mWall; }
    virtual const sBcPointData *getFootBgPointData() { return &getBgPointData()->mFoot; }
    virtual float getStandHeadBgPointY();
    virtual void checkBgCrossSub();
    virtual void postBgCross();
    virtual float getSandSinkRate();
    virtual bool setPressBgDamage(int, int);
    virtual bool setBalloonInDispOut(int);
    virtual bool isChange();
    virtual bool isEnableDokanInStatus();
    virtual bool setHideNotGoalPlayer();
    virtual int setDemoGoal(mVec3_c &landPos, float goalCastleX, u8 goalType);
    virtual bool setDemoCannonWarp(int, short, short);

    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, DemoStartWait);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, DemoDown);

    virtual void initialDoor();
    virtual void initialVine();
    virtual void initialJumpRight();
    virtual void initialJumpLeft();
    virtual void initialSwim();
    virtual void initialBlockJump();
    virtual void initialBlockJumpBelow();

    virtual void setCreateAction(int);
    virtual bool setTimeOverDemo();
    virtual void setFallDownDemo();
    virtual bool setDokanIn(DokanDir_e dir);
    virtual void initDemoOutDokan();
    virtual bool updateDemoKimePose(ClearType_e clearType);
    virtual void initDemoGoalBase();
    virtual void executeDemoGoal_Run();
    virtual void initializeDemoControl();

    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Walk);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Fall);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Land);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Crouch);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, SitJump);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Slip);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, HipAttack);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Swim);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Kani);
    STATE_VIRTUAL_FUNC_DECLARE(dAcPy_c, Cloud);

    virtual bool isWaitFrameCountMax();
    virtual bool checkWalkNextAction();
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
    virtual void setHipAttack_AttackStart();

    virtual float getCloudOffsetY();
    virtual bool setRideJrCrown(const dActor_c *);
    virtual bool isRideJrCrownOwn(const dActor_c *);
    virtual void setRideJrCrownMtx(const mMtx_c *);
    virtual void setRideJrCrownAnm(int);

    virtual const mVec3_c *getHeadTopPosP();
    virtual const float *getGravityData();
    virtual bool isCarry() const { return mCarryActorID != BASE_ID_NULL; }
    virtual bool isLiftUp();
    virtual bool isLiftUpExceptMame();

    virtual void setStar(daPlBase_c::StarSet_e, int);
    virtual void endStar();
    virtual void setVirusStar(daPlBase_c *);

    virtual bool isNoDamage();
    virtual bool setDamage(dActor_c *, DamageType_e);
    virtual bool setForcedDamage(dActor_c *, DamageType_e);
    virtual bool setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode);
    virtual bool _setJump(float jumpSpeed, float speedF, bool allowSteer, int keyMode, int jumpMode);

    virtual bool setHipAttackOnEnemy(mVec3_c *hitPos);

    virtual void clearJumpActionInfo(int);
    virtual bool setSwimSpeed(float speedY, float speedF);

    virtual void set1UpKinokoEffect();
    virtual void setFlagGetEffect();
    virtual void setHipAttackEffect();

    virtual void startQuakeShock(dQuake_c::TYPE_SHOCK_e);
    virtual void startPatternRumble(const char *pattern);

    virtual bool setDamage2(dActor_c *, daPlBase_c::DamageType_e);

    virtual bool isCarryMamePlayer();

    STATE_FUNC_DECLARE(dAcPy_c, RideOffJump);
    STATE_FUNC_DECLARE(dAcPy_c, SpinHipAttack);
    STATE_FUNC_DECLARE(dAcPy_c, RollSlip); ///< Flying through the air after being thrown by another player.
    STATE_FUNC_DECLARE(dAcPy_c, Vine);
    STATE_FUNC_DECLARE(dAcPy_c, Hang);
    STATE_FUNC_DECLARE(dAcPy_c, Pole);
    STATE_FUNC_DECLARE(dAcPy_c, Fire);
    STATE_FUNC_DECLARE(dAcPy_c, LiftUp); ///< Lifting up another player.
    STATE_FUNC_DECLARE(dAcPy_c, Throw);
    STATE_FUNC_DECLARE(dAcPy_c, PropelThrow);
    STATE_FUNC_DECLARE(dAcPy_c, WallJump);
    STATE_FUNC_DECLARE(dAcPy_c, WallSlide);
    STATE_FUNC_DECLARE(dAcPy_c, Propel);
    STATE_FUNC_DECLARE(dAcPy_c, CarryPlayer); ///< Being carried by another player.
    STATE_FUNC_DECLARE(dAcPy_c, RideYoshi);
    STATE_FUNC_DECLARE(dAcPy_c, SpinJump);
    STATE_FUNC_DECLARE(dAcPy_c, PenguinSlide);
    STATE_FUNC_DECLARE(dAcPy_c, KaniJump);
    STATE_FUNC_DECLARE(dAcPy_c, Quake);
    STATE_FUNC_DECLARE(dAcPy_c, ElecShock);
    STATE_FUNC_DECLARE(dAcPy_c, FlyDamage);
    STATE_FUNC_DECLARE(dAcPy_c, IceDamage);
    STATE_FUNC_DECLARE(dAcPy_c, CannonJump);
    STATE_FUNC_DECLARE(dAcPy_c, TarzanRope);
    STATE_FUNC_DECLARE(dAcPy_c, PlayerEat);
    STATE_FUNC_DECLARE(dAcPy_c, Balloon);
    STATE_FUNC_DECLARE(dAcPy_c, BlockJump); ///< Rescue Toad jumping out of his block.
    STATE_FUNC_DECLARE(dAcPy_c, JrCrown);
    STATE_FUNC_DECLARE(dAcPy_c, DemoInDoor);
    STATE_FUNC_DECLARE(dAcPy_c, DemoInJump);
    STATE_FUNC_DECLARE(dAcPy_c, DemoInVine);
    STATE_FUNC_DECLARE(dAcPy_c, DemoOutDoor);
    STATE_FUNC_DECLARE(dAcPy_c, DemoFallDown);
    STATE_FUNC_DECLARE(dAcPy_c, DemoFireDown);
    STATE_FUNC_DECLARE(dAcPy_c, DemoEatDie);
    STATE_FUNC_DECLARE(dAcPy_c, DemoDokanCannon);
    STATE_FUNC_DECLARE(dAcPy_c, DemoCannonWarp);

    void setSeaLandSmokeEffect();
    bool setHipAttackAction();
    void setWaterInEffect();
    void setSpinHipAttackEffect();
    void setScrollMode(s8);
    dActor_c *getCarryPropelActor();
    void setPropelSpinSmokeEffect();
    void setWaterWalkFlag();
    bool setKaniActionInitHangHand();
    bool jump_common();
    void _jumpSet(jmpInf_c *);
    void calcJumpCount();
    float getJumpSpeed();
    void setJumpSpeed();
    void fn_80127740(int jumpType, AnmBlend_e blendMode); ///< Jump animation set, some enum as first param @unofficial
    void fn_80145fd0(int jumpType); ///< Jump voice, some enum as param @unofficial
    bool checkCarryThrow();
    void jumpExeTakeOff();
    void jumpExecAir();
    void fn_801282d0(AnmBlend_e blendMode); ///< @unofficial
    bool spinLiftUp(dActor_c *, bool); ///< @unofficial
    void setJumpCommonBase();
    dAcPy_c *getCarryPlayer();
    bool isIceSlipAnmPlay();
    int checkWallSlideEnable(int);
    bool checkWallJump();
    void setWallSlideEffect();
    void setSpinHoldReq(float targetX);
    bool setSpinSpeed(float, float);
    void setSpinJumpEffect(int);
    void endSpinJumpEffect();
    void setSpinJumpEffectSub(int);
    void setMissSpinJumpEffect(int);
    void initRideSpinMove();
    void setSpinFireBall();
    void endRideSpinMove();
    bool isMissSpinEnableAnm();
    s16 getMissSpinAngle();
    void setMissSpin();
    void resetMissSpin();
    void updateMissSpin();
    void setKaniMoveSpeed(bool);
    void setCrouchSmokeEffect();
    void setSlipSE();
    void setSlipEffect();
    void releaseCarryActor();
    void setInvalidKeyTimer(int, int);
    void setInvalidKeyTimer_LR(int, int);
    bool checkPenguinSlideJump();
    void setPenguinSlideEffect();
    void setPenguinSlideLandEffect();
    void setNormalWalkAnm(AnmBlend_e, float);
    void setRunOnWaterEffect();
    void turnPowerSet();
    void setTurnSmokeEffect();
    void CrouchActionGround();
    void CrouchActionWater();
    void setWaterGroundJump();
    dActor_c *getCarryPropelBlock();
    dActor_c *getCarryHardBlock();
    float getLiftUpOffset();
    mVec3_c getLiftUpPos();
    void clearSpinLiftUpReserve();
    void checkSpinLiftUpReserve(dCc_c *cc);
    void setSpinLiftUpReserve();
    void checkSpinLiftUpRoofHeight();
    void setCarryOffFall(const dAcPy_c *player);
    bool checkEnableThrow();
    void initializeThrowCommonBase();
    void initializeThrowCommon();
    void finalizeThrowCommonBase();
    void finalizeThrowCommon();
    void setThrowActor();
    const sBcPlayerPointData *getBgPointData();
    void setBcData(int);
    void executeThrowCommon();
    void setWaterMoveSpeed();
    void calcUzuSwimSpeed(float, float, float *); ///< @unofficial
    void setUzuSpeedY(float f);
    void setUzuSpeedF(float f);
    bool setUzuSwimAction();
    bool setSwimAction();
    void setWaterOutEffect();
    void setPaddleSwimEffect();
    void resetPaddleSwimEffect();
    void setWaterSurfaceSwimEffect();
    void setFlutterKickEffect();
    void setInitSwimAction_FireBall();
    const sBcPlayerPointData *getBgPointData_Powerup(PLAYER_POWERUP_e, int); ///< @unofficial
    bool setWaterSurfaceJump();
    void createFireBall(int);
    void SwimAction_Swim();
    void setSwimAction_Swim(AnmBlend_e blendMode);
    void setSwimAction_Walk(AnmBlend_e blendMode);
    void setSwimAction_Penguin(AnmBlend_e blendMode);
    void setSwimActionWalkAnm();
    void SwimAction_Walk();
    s16 getPenguinSwinAngleX();
    void setPenWaterMoveSpeed(int i);
    bool setPenguinPaddleSwim();
    void SwimAction_Penguin();
    void setSwimAction_FireBall();
    void SwimAction_FireBall();
    bool checkSetFireBall();
    void calcPenguinSwimGroundRev();
    void setIvyHangEffect();
    bool setVineAction();
    float getHangBcOffsetY();
    bool setKaniHangToVineAction();
    bool setVineToKaniHangAction();
    void calcVineSpeed();
    bool checkVineEnd();
    void setVineWalkSE();
    void VineActionIvy();
    bool setRideOffPlayerJump(float, float);
    void VineActionNet();
    void VineActionAttack();
    void VineActionRoll();
    bool checkNetPunch();

    bool setAmiRollAction(mVec3_c *);
    bool isAmiRollAction();
    void setAmiRollPos(short, float);
    void setAmiRollPos(short, float, mVec3_c &);
    void endAmiRollAction(short ang);

    void setCarryPlayerHang(float height);
    bool setHangAction();
    void HangActionStart();
    void setHangActionWait();
    void HangActionWait();
    void setHangActionMove();
    void HangActionMove();

    void setPoleTurnAngle();
    bool setPoleAction();
    void PoleActionStart();
    void setPoleActionWait();
    void PoleActionWait();
    void setPoleActionUp();
    void PoleActionUp();
    void setPoleActionDown();
    void PoleActionDown();
    void setPoleJump();
    bool setPoleShakeJump();

    bool setKaniHangAction();
    void setKaniWalkEffect();
    void setKaniHangEffect();
    void setKaniCliffCatchEffect();
    void setKaniAction_Walk();
    void setKaniAction_Hang();
    void setKaniAction_HangUp();
    void setKaniAction_HangFall();
    void setKaniAction_JumpHang();
    void setKaniAction_HangUpVine();
    void setKaniAction_HangHand();
    void setKaniHangStartEffect();
    void KaniAction_Walk();
    void KaniAction_HangInit();
    void KaniAction_JumpHangInit();
    bool checkCliffHangFootGround();
    bool checkCliffHangWater();
    void KaniAction_Hang();
    void KaniAction_HangFall();
    void KaniAction_HangUp();
    void KaniAction_HangUpVine();

    void setCatchRopeSE();
    void setClimbRopeSE();
    void setSlideRopeSE();
    void setRopeHasigoSE();
    void releasePoleCheck();

    bool setTarzanRopeAction();
    bool setTarzanRopeJump();

    daYoshi_c *getRideYoshi();
    bool isNotBalloonCourse();
    bool fn_801477c0(); ///< @unofficial
    void setSceneChangeInfo();
    bool updateRopeAngle();
    void setRopeSwingAnm(float, float);
    bool checkStartSwingUp();
    bool checkStartSwingDown();
    void updateRopeSwingAnm();
    void TarzanRopeActionStart();
    void setTarzanRopeActionWait();
    void TarzanRopeActionWait();
    void setTarzanRopeActionUp();
    void TarzanRopeActionUp();
    void setTarzanRopeActionDown();
    void TarzanRopeActionDown();
    void setTarzanRopeActionHasigoMove();
    void TarzanRopeActionHasigoMove();
    void TarzanRopeActionHasigoHangDown();
    void setTarzanRopeActionHasigoHangUp();
    void TarzanRopeActionHasigoHangUp();

    void startQuakeAction(QuakeMode_e);
    void setQuakeAction(int, QuakeMode_e);
    void setQuakeActionLocal(int, QuakeMode_e);
    void endQuakeAction();
    void setQuakeNumbEffect();

    void initElecShock();
    bool executeElecShock();
    void setElecEffect();

    bool setFlyDamageAction(int, dActor_c *);

    void initCcData();
    void initBcData();
    void calcModel();

    void setBreakBalloonJump(u8, short);
    void setDrawBalloonInPlayer(mVec3_c &pos);
    bool checkAllBalloonFade();
    void setBalloonHelpVoice();
    bool setBalloonInNextgoto();
    bool setBalloonInDamage();
    bool setBalloonButtonA();
    bool setBalloonInDispOutByYoshi(int);
    bool setBalloonInDispOutBase(int, int);

    void setShakeIce(float);
    mVec3_c getIceDrawPos();

    void fn_801395a0(); ///< @unofficial
    void fn_801416c0(PLAYER_POWERUP_e); ///< @unofficial

    bool setRideOffYoshiJump(daPlBase_c *yoshi);
    void setRideOnYoshi(daYoshi_c *yoshi);

    void setCloudStateMove();
    bool checkCloudStateCrouch();
    void setCloudStateFireCreate();

    bool checkCarryActor(dAcPy_c *player);
    bool setDropCarryPlayer();
    void setCarryPlayerMode_Move(float);
    void setCarryPlayerMode_Fire();
    void setCarryPlayerMode_Crouch();

    dAcPy_c *getRidePlayer();

    void setYoshiBackPos();
    void setPlayerHandPos();
    void setPlayerEatPos(dActor_c *eatingActor);
    void setPlayerEatReact();

    void endPowerUpEffect();

    void setJrCrownPos();
    void setJrCrownElecDamage();

    bool setFireBallAction();
    bool checkFireJump();

    bool fn_80146e40(float, float, bool); ///< @unofficial
    int getSpinActionMode();

    bool setPropelAction();
    void resetPropelFlyTime();
    void clearPropelFlyUpTime();
    void calcPropelMoveSpeedF();
    void setPropelActionFlyInit();
    void setPropelActionBigFly();
    void setPropelActionFly();
    void PropelActionFly();
    void setPropelActionFall();
    void PropelActionFall();
    void setStartPropelJumpEffect();
    bool calcPropelFallSpinEffect();
    void setPropelFallSpinEffect();
    void endPropelFlyPartsMove();
    void stopPropelFallSpinEffect();
    void updatePropelParts();
    void updatePropelFlyPartsMove();
    bool isEnablePropelJump();
    bool setSpinActionReq();
    bool setSpinAction();

    void initPlayerDownCommon();
    void initDemo_DownPose();
    void initDemo_DownFall();
    void exeDemo_DownFall();
    void checkRest();
    void stopOtherDownDemo();
    void playOtherDownDemo();
    void setPlayerEatDie();
    void releaseEatDie();
    bool setEatDieHide();
    bool setEatDieFadeOut();
    bool setEatDieSpitOut();
    void setEatDieScreamVoice();

    bool searchDoorActor();
    void setDemoInDoor_Walk();
    bool isDoorDemoEnable();
    bool setDoorDemo(dActor_c *);
    void exeDemoOutDoor_OpenDoor();
    void initDemoOutDoor_MoveInter();
    void exeDemoOutDoor_MoveInter();
    void initDemoOutDoor_MoveCenter();
    void exeDemoOutDoor_MoveCenter();
    void exeDemoOutDoor_WaitClose();
    void exeDemoOutDoor_WaitEnter();

    bool initDemoDokanCannon(mVec3_c &pos, int cannonMode); ///< @unofficial
    void endDemoDokanCannon(mVec3_c &pos);
    bool isDokanCannonIn();
    bool setCannonJump(float speedY, float speedF, int jumpType);

    void setOffYoshiInGoal(daPlBase_c *yoshi);

    bool isDrawingCarryFukidashi();
    void getCcBounds(sRangeDataF *bounds); ///< @unofficial
    bool cancelCarry(dActor_c *carriedActor);

    void setFireBallDamage(u8 type);

    bool fn_80145c00(PLAYER_POWERUP_e); ///< @unofficial

    mVec3_c getModelScaleBase();
    mVec3_c getModelScale();

    void calcInvalidKeyTimer();
    void calcTimerProc();
    void calcStarTimer();

    void initChangeInit();
    bool executeChangeInit();
    int change_reverse_scale_set();
    void initChangeNormal();
    bool executeChangeNormal();
    void setChange(int);
    bool updateChange();

    void maxFallSpeedSet();
    void setFollowEffect(int);
    void setItemGetEffect();
    void setChukanPowerUpEffect();
    void updateItemGetEffect();
    void updateFollowEffect();

    void setCenterOffset();
    void selectAction();

    bool isDrawingFukidashi();
    void managePropelFukidashi();

    void setCcAtSpin();
    void setCcAtSpinFall();
    void setCcAtSpinLiftUp();
    void setCcAtNetPunch();
    void setCcData();
    void setCcDataDirect();

    void fn_80143060(sBcPointData &data1, sBcPointData &data2, bool); ///< @unofficial
    bool fn_80143220(sBcPointData &data1, sBcPointData &data2); ///< @unofficial
    void reviseBcDataCarryPlayer(sBcPointData &data1, sBcPointData &data2); ///< @unofficial
    void reviseBcDataCarryHardBlock(sBcPointData &data1, sBcPointData &data2); ///< @unofficial

    void setStarBase(StarSet_e, int);

    bool searchKinopioNearPlayer(float *dist);

    void setPlayerData();
    void setSpeedData();
    void setModeGravity();

    static bool ccCheckAttack(dCc_c *self, dCc_c *other);
    static bool ccCheckStamp(dCc_c *self, dCc_c *other);
    static bool ccCheckSideHit(dCc_c *self, dCc_c *other);
    static void ccCallBack(dCc_c *self, dCc_c *other);
    static void atCcCallBack(dCc_c *self, dCc_c *other);

    mVec3_c getCarryPos();

    mMtx_c getCarryMtx() {
        mMtx_c mtx;
        mtx.trans(getCarryPos());
        mtx.concat(getModel()->getMtx());
        return mtx;
    }
    PLAYER_POWERUP_e getPowerupCopy() { return mPowerupCopy; }
    bool isClimbing() {
        return isStatus(STATUS_36) ||
            isStatus(STATUS_VINE) ||
            isStatus(STATUS_HANG) ||
            isStatus(STATUS_KANI_HANG) ||
            isStatus(STATUS_POLE);
    }
    float getDirSpeed() const {
        return sc_DirSpeed[mDirection];
    }

    int mIsRescueKinopio;
    PLAYER_POWERUP_e mPowerupCopy;
    PLAYER_POWERUP_e mPowerupCopy2;
    mVec2_c m_0c;
    sBcPointData mVineBcData;
    dPc_c mPc;
    int m_60;
    s8 mScrollMode;
    s8 mChangeType;
    s8 mChangeTimer;
    s8 m_67;
    int m_68;
    float mAmiRelated2;
    int mDoorSize;
    int m_74;
    int m_78;
    int m_7c;
    int m_80;
    int mWaitFrameCount;
    int m_88;
    int mJumpCounter;
    u8 mJumpComboTimer; ///< Timer for allowing a slight delay between jumps to still count as a combo.
    u8 mWallSlideCooldown; ///< Timer to disable wall sliding while active.
    float m_94;
    mEf::levelEffect_c mLevelEf1;
    mEf::levelEffect_c mLevelEf2;
    mAng m_2e8;
    int m_2ec;
    int m_2f0;
    s16 m_2f4;
    s16 mPropelRollSpeed;
    s16 mIsPropelFall;
    mEf::levelEffect_c mLevelEf3;
    dEf::followEffect_c mFollowEf;
    int m_538;
    int m_53c;
    int m_540;
    int m_544;
    int m_548;
    float m_54c;
    mEf::levelEffect_c mLevelEf4;
    mEf::levelEffect_c mLevelEf5;
    u8 mPad1[0x4];
    mVec3_c mAmiRollPos;
    float mAmiXDiff;
    float mAmiRelated;
    s16 mAmiAng;
    int m_7bc;
    u16 m_7c0;
    u32 m_7c4;
    mEf::levelEffect_c mLevelEf6;
    u8 m_8f0;
    mVec2_c m_8f4;
    int mPoleGrabCooldown; ///< Timer to disable grabbing a pole while active.
    short m_900, m_902, m_904, m_906;
    int m_908;
    RopeSwingState_e m_90c;
    int mTarzanRopeCooldown; ///< Timer to disable swinging on a vine while active.
    int m_914;
    float m_918;
    int m_91c;
    mEf::levelEffect_c mLevelEf7;
    mEf::levelEffect_c mLevelEf8;
    int m_b70;
    int m_b74;
    float m_b78;
    int m_b7c;
    int m_b80;
    int m_b84;
    u8 m_b88;
    u8 m_b89;
    float m_b8c;
    mVec2_c mUzuSwimSpeed;
    int m_b98;
    mEf::levelEffect_c mLevelEf9;
    mEf::levelEffect_c mLevelEf10;
    mEf::levelEffect_c mLevelEf11;
    mEf::levelEffect_c mLevelEf12;
    int m_103c;
    int m_1040;
    int mSpinTimer; ///< Timer for the duration of a spin jump.
    s16 m_1048;
    u8 mPad2[0x4];
    float mSpinHoldReqTarget; ///< The target X position to move to while doing a spin in place.
    u32 mSpinFireBallCooldown; ///< Timer to control how often fireballs the player automatically shoots while spinning.
    u32 m_1058;
    s16 m_105c;
    dEf::followEffect_c mFollowEf2;
    dEf::followEffect_c mFollowEf3;
    u32 m_1288;
    u32 m_128c;
    u8 mPad3[0x8];
    int m_1298;
    mVec2_c m_129c;
    u8 mPad4[0x4];
    mVec3_c m_12a8;
    u32 m_12b4;
    int m_12b8;
    u32 mAllBalloonFadeTimer;
    mMtx_c mRideJrClownMtx;
    int mSpinCooldown; ///< Timer to disable another spin action while active.
    u8 m_12f4;
    float m_12f8;
    fBaseID_e m_12fc;
    float m_1300;
    float m_1304;
    int m_1308;
    int m_130c;
    int m_1310[2];
    int m_1318[2];
    int mFollowType;
    int mPowerUpEffectTimer; ///< Timer controlling the powerup effect duration and opacity.
    int mPowerUpType; ///< 0: Touching an item, 1: Touching the midway point.
    dEf::dLevelEffect_c mItemGetEffect1;
    dEf::dLevelEffect_c mItemGetEffect2;
    int m_157c;
    int m_1580;
    int m_1584;
    dPyMdlMng_c mPyMdlMng;
    float m_1594;
    float m_1598;
    float m_159c;
    fBaseID_e mCarryActorID;
    int mFastRunFrames; ///< How many frames the player has been running at over 1 unit/frame.
    int m_15a8;
    /// Disables interaction with other players and enemies while the timer is non-zero.
    short mNoInteractTimer;
    short m_15ae;
    short m_15b0;
    short m_15b2;
    short mWaterWalkTimer;
    short m_15b6;
    short m_15b8;
    short mBalloonHelpVoiceCooldown; ///< Timer to only allow the help voice to play every 2 seconds.
    dEf::followEffect_c mFollowEf4;
    dPropelParts_c mPropelParts;
    dPlayerOrchestra_c mPlayerOrchestra;

    ACTOR_PARAM_CONFIG(PlayerNo, 0, 4);
    ACTOR_PARAM_CONFIG(CreateAction, 16, 8);
    ACTOR_PARAM_CONFIG(Direction, 24, 1);

    static const float data_802f5a0c[];
    static const float data_802f5a48[];
    static dAcPy_HIO_Speed_c m_speed_hio[2];
    static const float msc_JUMP_SPEED;
};
