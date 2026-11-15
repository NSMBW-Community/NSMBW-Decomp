#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_ice_effect_maker.hpp>
#include <game/bases/d_ice_manager.hpp>
#include <game/bases/d_ice_param.hpp>

/// @brief Ice blocks for enemies.
/// @statetable
/// @paramtable
class daIce_c : public dActorState_c {
public:
    /// @brief The possible pre-defined ice block types.
    /// @unofficial
    enum IceType_e {
        ICE_TYPE_SQUARE, ///< A square block.
        ICE_TYPE_TATE, ///< A vertical block.
        ICE_TYPE_YOKO, ///< A horizontal block.
        ICE_TYPE_BIG_SQUARE, ///< A large square block.
        ICE_TYPE_BIG_TATE, ///< A large vertical block.
        ICE_TYPE_BIG_YOKO, ///< A large horizontal block.
        ICE_TYPE_COUNT
    };

    /// @brief How the ice block behaves when it's created.
    /// @unofficial
    enum InitType_e {
        INIT_LAND, ///< The block rests on the ground. @see StateID_Land
        INIT_FREEZE, ///< The block is frozen in place indefinitely. @see StateID_Freeze
        INIT_AIR, ///< The block hangs in the air for a while, then falls down. @see StateID_Air
        INIT_FALL, ///< The block falls down immediately. @see StateID_Fall
        INIT_WATER, ///< The block was created underwater and floats up. @see StateID_Sink
        INIT_WATER_TOGE, ///< Same as #INIT_WATER, but for spiky enemies that break the ice block after a while.
        INIT_WATER_STUCK, ///< The block was created underwater, but doesn't float up.
        INIT_COVER, ///< The block follows the parent enemy. @see StateID_Cover
        INIT_HOP ///< The block is launched with the parent enemy's speed. @see StateID_Hop
    };

    /// @brief The weight class of the ice block. Heavy blocks cannot be carried.
    /// @unofficial
    enum IceWeight_e {
        ICE_WEIGHT_LIGHTEST,
        ICE_WEIGHT_LIGHT,
        ICE_WEIGHT_HEAVY,
        ICE_WEIGHT_HEAVIEST,
        ICE_WEIGHT_COUNT
    };

    /// @brief The tile collision mode of the ice block.
    /// @unofficial
    enum CollisionMode_e {
        COLLISION_NORMAL, ///< The block collides with tiles.
        COLLISION_NONE, ///< The block does not collide with tiles.
        COLLISION_NO_SINK_COLLISION ///< The block does not check for ground collision while sinking in water.
    };

    /// @brief Flags for #mFlags.
    /// @unofficial
    enum Flags_e {
        FLAG_BREAKABLE = BIT_FLAG(0), ///< The block can be broken by ground pounding or attacking it from below.
        FLAG_CARRYABLE = BIT_FLAG(1) ///< The block can be picked up by players.
    };

    /// @brief How the ice block is pushed under the water surface.
    /// @unofficial
    enum PushSinkType_e {
        PUSH_SINK_LAND, ///< A player landed on the block.
        PUSH_SINK_HIP_ATTACK, ///< A player ground-pounded the block.
        PUSH_SINK_PRESS, ///< A player standing on the block is pressed against a ceiling.
        PUSH_SINK_COUNT
    };

    /// @brief The state of the water surface while the block floats on top of it. See StateID_FloatWait.
    /// @unofficial
    enum FloatState_e {
        FLOAT_STATE_INIT, ///< The water level has not been checked yet.
        FLOAT_STATE_STILL, ///< The water level is constant, the block bobs up and down.
        FLOAT_STATE_RISING, ///< The water level is rising.
        FLOAT_STATE_LOWERING ///< The water level is lowering.
    };

    /// @brief The result of checkPlAttach_Wall().
    /// @unofficial
    enum WallAttachCheckResult_e {
        WALL_ATTACH_NONE, ///< The player is not touching a wall.
        WALL_ATTACH_TOUCH, ///< The player is also touching a wall on the other side of the ice block.
        WALL_ATTACH_STUCK, ///< The player is stuck between the ice block and a wall.
    };

    typedef void (daIce_c::*EnlargeFunc)();

    daIce_c() : mShake(0), mFlags(0), mBreakRequested(0), mIceEfMaker(this) {} ///< @copydoc dActorState_c::dActorState_c
    ~daIce_c() {} ///< @copydoc dActorState_c::~dActorState_c

    int create() override;
    int doDelete() override;
    int execute() override;
    int preExecute() override;
    int draw() override;
    void deleteReady() override;
    void finalUpdate() override;
    void block_hit_init() override;
    bool isSpinLiftUpEnable() override { return canCarry(); }
    void setSpinLiftUpActor(dActor_c *carryingActor) override { setCarry(carryingActor); }
    bool vf68(dBg_ctr_c *collider) override;

    /// @brief Returns whether touching the collider beneath the ice block counts as touching the ground.
    /// @unofficial
    virtual bool checkFootBgcIsGround();

    void createMdl(); ///< Creates the model and texture animation according to the ice type.
    void calcMdl(); ///< Updates the model matrix.

    /// @brief Sets up the ice block. Called by the ice manager after creation.
    /// @param size The scale of the ice block, relative to the default size of the ice type.
    /// @param scale The scale of the effects.
    void setInfo(mVec3_c size, dIceEfScale_c scale);

    void initSize(const mVec3_c &scale); ///< Calculates the block's size from the ice type and the given scale.
    void initPos(); ///< Places the block on the ground if it's landing or falling.
    void initWeight(); ///< Calculates the weight class of the block. @see IceWeight_e
    void initCollision(); ///< Sets up the tile collision, the actor collision and the dynamic collider.
    void initEffect(dIceEfScale_c scale); ///< Initializes the effect maker.

    /// @brief Checks whether the ice block should be broken immediately after creation.
    /// @param checkBg Whether to also check for overlap with background tiles.
    /// @return Whether a player (or optionally a tile) is inside the ice block.
    bool checkInstantBreak(BOOL checkBg);
    bool checkPlayerIn(); ///< Checks whether a player overlaps the ice block.
    bool checkBgIn(); ///< Checks whether a solid tile overlaps the ice block.

    /// @brief Calculates the highest ground position below the ice block.
    /// @param height Receives the ground height, if found.
    /// @return Whether the ice block would clip into the ground in its current position.
    bool calcLandLine(float &height);

    bool isQuakeDamage(); ///< Checks whether the ice block should be broken by an earthquake.
    void quakeDamage(); ///< Awards the earthquake combo score and breaks the ice block.

    /// @brief Requests the ice block to be broken.
    /// @param dir The direction the enemy flies off in.
    /// @param playerNo The player that broke the ice block, or -1. Receives a score if valid.
    void breakReq(int dir, int playerNo);
    void floatbreakReq(); ///< Requests the ice block to break on water. No score is awarded.

    void combobreakReq(int dir, dActor_c *initiator); ///< Requests the ice block to be broken by a combo attack (e.g. a shell being thrown).
    void plcombobreakReq(int dir, dActor_c *initiator); ///< Like combobreakReq(), but uses the player's combo counter.
    void treadcombobreakReq(int dir, dActor_c *initiator); ///< Like combobreakReq(), but uses the stomp combo score.
    void starbreakReq(int dir, dActor_c *initiator); ///< Like combobreakReq(), but uses the star combo score.

    void setBc(); ///< Sets up the tile collision sensors.
    void setCarryBc(); ///< Sets up the tile collision sensors while the block is being carried.
    void enlargeBgc(); ///< Enlarges the dynamic collider until it reaches the full size.
    void enlargeBgcLand(); ///< Enlarges the dynamic collider when not in water.
    void enlargeBgcWater(); ///< Enlarges the dynamic collider when in water.

    void enemyAttach(); ///< Moves the parent enemy along with the ice block.
    void shake(); ///< Shakes the model of the ice block.
    void normalShake(); ///< Shakes the model left and right.
    void watertogeShake(); ///< Swings the model left and right with increasing frequency. Used for #INIT_WATER_TOGE.

    void calcFloatAngle(); ///< Tilts the ice block back and forth while floating up.
    void calcFloatSpeed(); ///< Accelerates the ice block towards #mFloatTargetSpeed in the direction it is tilted to.

    /// @brief Checks whether the given actor should push the ice block down and sets #mPushSinkType accordingly.
    /// @param actor The actor to check.
    /// @return Whether the ice block should be pushed down.
    bool checkPushSink(dActor_c *actor);

    dIceMng_c *getManager(); ///< Gets the ice manager of the parent enemy.

    bool checkUnderBreak(dActor_c *actor); ///< Checks whether a falling block hits a player / Yoshi from above and causes daPlBase_c::DAMAGE_HEAD_BONK.
    bool checkUpperBreak(dActor_c *actor); ///< Checks whether an actor landing on the block breaks it. [Always false].
    bool checkSideBreak(dActor_c *actor, int direction); ///< Checks whether a thrown block hits a player / Yoshi from the side and causes daPlBase_c::DAMAGE_HIP_ATTACK.

    void breakEffect(); ///< Plays the break effect.
    void meltEffect(); ///< Plays the melt effect.
    void yoganEffect(); ///< Plays the lava splash effect.
    void poisonEffect(); ///< Plays the poison splash effect.

    bool checkNetIn(); ///< Checks whether the ice block overlaps a climbable fence.

    void setCarry(dActor_c *actor); ///< Starts being carried by the given actor.
    mVec3_c calcCarryPos(); ///< Calculates the position of the block in the carrying player's hands.
    BOOL canCarry() const; ///< Checks whether the ice block can currently be picked up.

    /// @brief Returns whether a player is stuck between the ice block and the background collision.
    /// @param plrNo Receives the player number, or -1 if no player is stuck.
    bool checkPlHasami(int *plrNo);

    /// @brief Returns whether a player is standing on the ice block.
    /// @param playerTouchingCeiling If not @p nullptr, receives @p TRUE if the player is also touching the ceiling, else @p FALSE.
    bool checkPlAttach_Head(BOOL *playerTouchingCeiling);

    /// @brief Returns whether a player's head is touching the ice block.
    /// @param playerTouchingFloor If not @p nullptr, receives @p TRUE if the player is also touching the floor, else @p FALSE.
    bool checkPlAttach_Foot(BOOL *playerTouchingFloor, int *plrNo);

    /**
     * @brief Returns whether a player is touching the given side of the ice block.
     * @param dir The side of the ice block to check.
     * @param checkResult If not nullptr, receives a value of type #WallAttachCheckResult_e.
     */
    bool checkPlAttach_Wall(int dir, int *checkResult);

    /// @brief Checks whether the given player is stuck between the ice block and a wall.
    /// @param dir The side of the ice block to check.
    bool checkPlHasami_Wall(dActor_c *actor, int dir);

    fBaseID_e getParentID(); ///< Gets the ID of the parent enemy.

    STATE_FUNC_DECLARE(daIce_c, Freeze); ///< Frozen in place.
    STATE_FUNC_DECLARE(daIce_c, Cover); ///< Following the parent enemy. [Used for Fuzzies].
    STATE_FUNC_DECLARE(daIce_c, Hop); ///< Being launched with the parent enemy's speed. [Used for the Pokey segments].
    STATE_FUNC_DECLARE(daIce_c, Sink); ///< Slowing down after being created underwater.
    STATE_FUNC_DECLARE(daIce_c, PushSink); ///< Being pushed under the water surface.
    STATE_FUNC_DECLARE(daIce_c, Float); ///< Floating up to the water surface while tilting back and forth.
    STATE_FUNC_DECLARE(daIce_c, SmallFloat); ///< Floating back up to the water surface after a small push downwards.
    STATE_FUNC_DECLARE(daIce_c, FloatWait); ///< Floating on the water surface.
    STATE_FUNC_DECLARE(daIce_c, TogeFloat); ///< Floating up to the water surface, variant for spiky enemies.
    STATE_FUNC_DECLARE(daIce_c, ShakeFloat); ///< Floating up slowly while shaking before melting.
    STATE_FUNC_DECLARE(daIce_c, Air); ///< Hanging in the air before falling down.
    STATE_FUNC_DECLARE(daIce_c, Fall); ///< Falling down.
    STATE_FUNC_DECLARE(daIce_c, Land); ///< Resting on the ground.
    STATE_FUNC_DECLARE(daIce_c, Dive); ///< Slowing down after falling into water.
    STATE_FUNC_DECLARE(daIce_c, YoganDive); ///< Sinking into lava or poison.
    STATE_FUNC_DECLARE(daIce_c, Carry); ///< Being carried by a player.
    STATE_FUNC_DECLARE(daIce_c, Throw); ///< Sliding across the ground after being thrown.
    STATE_FUNC_DECLARE(daIce_c, Drop); ///< Falling down after being dropped. Breaks on landing.
    STATE_FUNC_DECLARE(daIce_c, Melt_Normal); ///< Breaking apart after the ice block has has melted.

    float getFloatPosY() { return mWaterHeight - mSize.y * 0.5f; }
    bool isDestroy() { return mBreakRequested != 0 || mpIceMng != nullptr && mpIceMng->mDestroyMode != dIceMng_c::DESTROY_NONE; }

    static void callBackFoot(dActor_c *self, dActor_c *other); ///< Called when another actor's foot sensor touches the ice block.
    static void callBackHead(dActor_c *self, dActor_c *other); ///< Called when another actor's head sensor touches the ice block.
    static void callBackWall(dActor_c *self, dActor_c *other, u8 dir); ///< Called when another actor's wall sensor touches the ice block.

    static bool checkRevFoot(dActor_c *self, dActor_c *other); ///< Returns whether the ice block should push up the actor standing on it.
    static bool checkRevHead(dActor_c *self, dActor_c *other); ///< Returns whether the ice block should push down the actor below it.
    static bool checkRevWall(dActor_c *self, dActor_c *other, u8 dir); ///< Returns whether the ice block should push the actor at its side.

    static void ccCallBack(dCc_c *self, dCc_c *other); ///< The actor collision callback.
    static void ccCallBack_Water(dCc_c *self, dCc_c *other); ///< The actor collision callback in water.

private:
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFileMdl; ///< The resource file.
    m3d::mdl_c mMdl; ///< The model.
    nw4r::g3d::ResAnmTexSrt mResFileAnm; ///< The texture animation resource file.
    m3d::anmTexSrt_c mAnm; ///< The texture animation.
    dBg_ctr_c mBgCtr; ///< The dynamic collider.
    mVec2_c mCurrBgcSize; ///< The current dynamic collider size, grows over time.
    mVec2_c mBgcSizeIncrease; ///< The amount to increase #mCurrBgcSize per frame until the full size is reached.
    sBcMany mBcPointFoot; ///< The foot sensor for tile collision.
    sBcMany mBcPointHead; ///< The head sensor for tile collision.
    sBcMany mBcPointWall; ///< The wall sensor for tile collision.
    mVec3_c mPosDiff; ///< The offset to the parent actor.
    mVec3_c mInitialPos; ///< The position at creation.
    mVec3_c mModelPos; ///< The position of the model, including the shake offset.
    mVec3_c mPrevPos; ///< The position on the previous frame.
    mVec3_c mSize; ///< The size of the ice block.

    PushSinkType_e mPushSinkType; ///< How the block got pushed under the water surface.

    int mCarriedByPlrNo; ///< The player number of the player carrying the ice block.

    float mFloatTargetSpeed; ///< The target speed when floating.
    float mFloatAccel; ///< The acceleration towards #mFloatTargetSpeed.
    short mFloatAngle; ///< The current angle while floating up.
    u8 mPad1[0x4];
    float mExtraFloatSpeedX; ///< Added to the X speed while floating. @unused

    BOOL mShake; ///< Whether the ice block should shake.
    short mShakeRotSpeed; ///< The angular speed of #mShakeRotAngle.
    short mShakeRotAngle; ///< The current angle of the swinging motion.
    short mShakeRotAmplitude; ///< The maximum rotation of the swinging motion.

    u8 mPad2[0x4];

    BOOL mIsWater; ///< Whether the block was created in water.
    short mCurrFloatUpDown; ///< Counter for bobbing up and down on the water surface.
    float mWaterHeight; ///< The height of the water surface.
    FloatState_e mFloatState; ///< The state of the water surface, used in StateID_FloatWait.

    float mStartFallY; ///< The Y position where the block started falling, used to check whether the block falls far enough to break.
    int mCollEntryTimer; ///< Countdown until the collision is fully activated.
    int mTimer; ///< A general purpose timer.
    u32 mFlags; ///< Value is a FLAG_e.
    BOOL mBreakRequested; ///< Whether the ice block should break.
    float mPrevPosX; ///< The X position before running execute().

    InitType_e mInitType; ///< The initial behavior. @see InitType_e
    IceWeight_e mWeightType; ///< The weight class. @see IceWeight_e

    dIceMng_c *mpIceMng; ///< The ice manager.
    dIceEfMaker_c mIceEfMaker; ///< The ice effect maker.

    ACTOR_PARAM_CONFIG(IceType, 0, 4); ///< The shape of the ice block. See IceType_e.
    ACTOR_PARAM_CONFIG(Unbreakable, 8, 1); ///< If not set, sets #FLAG_BREAKABLE.
    ACTOR_PARAM_CONFIG(NoCarry, 12, 1); ///< If not set, sets #FLAG_CARRYABLE.
    ACTOR_PARAM_CONFIG(CollisionMode, 16, 2); ///< The tile collision mode. See CollisionMode_e.
    ACTOR_PARAM_CONFIG(NoQuakeScore, 20, 1); ///< Whether to not award a score when broken by an earthquake.
    ACTOR_PARAM_CONFIG(InitType, 24, 4); ///< The initial behavior. See InitType_e.

    static const short smc_OBJBG_DUMMY_ANGLE = 0;
};
