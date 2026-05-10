#include <game/bases/d_a_en_shell.hpp>
#include <game/mLib/m_effect.hpp>
#include <lib/MSL/string.h>

/**
 * @brief Koopa Troopa
 * @statetable
 * @paramtable
 */
class daEnNoko_c : public daEnShell_c {
public:
    class nodeCallback_c : public m3d::mdl_c::callback_c {
    public:
        nodeCallback_c(daEnNoko_c *owner) : mpOwner(owner) {}
        virtual void timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl);

        daEnNoko_c *mpOwner;
    };

    enum NokoType_e {
        NOKO_GREEN, ///< Green Koopas don't turn around on ledges.
        NOKO_RED ///< Red Koopas turn around on ledges.
    };

    enum SPAWN_MODE_e {
        SPAWN_MODE_WALK,
        SPAWN_MODE_SLEEP,
    };

    enum DANCE_MOVE_e {
        BGM_anim_walkA_1,
        BGM_anim_walkA_2, ///< Actually uses the same animation as BGM_anim_walkA_1.
        BGM_anim_walkA_3,
        DANCE_MOVE_COUNT,
    };

    daEnNoko_c() : mMdlCallback(this) {}

    int create() override;
    int doDelete() override;
    int execute() override;
    int preExecute() override;
    int draw() override;
    void finalUpdate() override { calcMdl(); }
    bool createIceActor() override;
    void beginFunsui() override;
    void endFunsui() override;

    STATE_FUNC_DECLARE(daEnNoko_c, BlockAppear); ///< Spawning from a block.
    STATE_FUNC_DECLARE(daEnNoko_c, Walk); ///< Walking on the ground.
    STATE_FUNC_DECLARE(daEnNoko_c, Turn); ///< Turning around while walking.
    STATE_FUNC_DECLARE(daEnNoko_c, WindTurn); ///< Being turned around by the wind.
    STATE_FUNC_DECLARE(daEnNoko_c, SpitOut_Ready); ///< About to be spit out by Yoshi.
    STATE_FUNC_DECLARE(daEnNoko_c, BgmDance); ///< Doing a dance move to the background music.
    STATE_FUNC_DECLARE(daEnNoko_c, BgmDanceEd); ///< Returning from a dance move to walking.
    STATE_VIRTUAL_FUNC_DECLARE(daEnNoko_c, Wakeup);
    STATE_VIRTUAL_FUNC_DECLARE(daEnNoko_c, WakeupTurn);

    bool setPlayerDamage(dActor_c *actor) override;
    bool checkSleep() override;
    bool turnProc() override;
    void calcShellEffectPos() override;

    void setEnemyTurn() override {
        if (isState(StateID_Walk)) {
            changeState(StateID_Turn);
        }
    }

    virtual void checkWaterEntry() { WaterCheck(mPos, 1.0f); }

    void setAfterSleepState() override { changeState(StateID_Walk); }

    void slideEffect() override {
        if (!mNokoType) {
            mEffect.createEffect("Wm_en_shellgreentail", 0, &mSlideEffectPos, nullptr, nullptr);
        } else {
            mEffect.createEffect("Wm_en_shellredtail", 0, &mSlideEffectPos, nullptr, nullptr);
        }
    }

    virtual void doTurn(int *dir, s16 *turnSpeed);
    virtual bool isWalking() { return true; }
    virtual void turnAround();
    virtual void calcMdl();
    virtual bool canDance();
    virtual void danceWithMove(int move);
    virtual void walkTurn();

    /// @brief Initializes the state of the Koopa after creation.
    virtual void setInitialState();

    /// @brief Subclasses can override this function to create additional models.
    virtual void createModelExtra() {}
    virtual void vf328() {}

    /// @brief Subclasses can override this function to delete additional resources.
    virtual void deleteResExtra() {}

    virtual mVec3_c getPos() { return mVec3_c(mPos.x, mPos.y, mPos.z); }

    BOOL isFunsui() const override { return mIsFunsui; }

    void createModel();
    void updateAmiLine();
    void setZPos();
    bool checkRyusa(); ///< Checks if the actor is inside quicksand.
    void ryusaEffect(); ///< Creates the quicksand effect.
    bool playerDamageTurn(dActor_c *); ///< Returns whether the Koopa should turn around after damaging the player.
    void setNokoBc(); ///< Reverts the collision to a regular Koopa.
    float getWindStrength();
    void setMoveAnimation(const char *name, m3d::playMode_e mode, float frame);
    void setBaseAnimation(const char *name, m3d::playMode_e mode, float frame);
    bool checkLedge(); ///< Checks if there is a ledge in front of the Koopa.
    void landEffect();

    u8 mPad0[4];

    dHeapAllocator_c mNokoAllocator; ///< The allocator used for the resources of this actor.
    nw4r::g3d::ResFile mNokoResFile; ///< The resource file containing the resources of this actor.
    m3d::mdl_c mNokoModel; ///< The model of the Koopa.
    m3d::anmChr_c mWalkAnim; ///< The walk animation of the Koopa.
    nw4r::g3d::ResAnmTexPat mNokoResAnmTex; ///< The animated texture resource of the Koopa.
    m3d::anmTexPat_c mNokoAnimTex; ///< The animated texture of the Koopa.

    BOOL mIsFunsui; ///< Whether the Koopa is being blown upwards by a fountain.
    s16 mHeadAngle;
    u8 mNokoType; ///< Is a NOKO_TYPE_e.
    mVec3_c mCreatePos; ///< The position where the Koopa was spawned.
    float mXSpeedBeforeFunsui; ///< The horizontal speed of the Koopa before being blown by a fountain.
    u8 mPad1[8];
    float mBaseZPos;
    u32 m_8c8;
    int mDanceTimer; ///< Timer for starting and ending the dance.
    mAng mBgmDanceRotSpeed; ///< The rotation speed for turning toward the target rotation for a dance.
    u32 mDanceMove; ///< The dance move to perform.
    mEf::levelOneEffect_c mQuicksandEffect;
    nodeCallback_c mMdlCallback;

    ACTOR_PARAM_CONFIG(NokoType, 0, 1); ///< See NOKO_TYPE_e.
    ACTOR_PARAM_CONFIG(SpawnMode, 4, 1); ///< See SPAWN_MODE_e.
    ACTOR_PARAM_CONFIG(SubLayer, 16, 1); ///< The sublayer to spawn on.
    ACTOR_PARAM_CONFIG(BlockHitPlayer, 24, 2); ///< The player number that hit the block from which this Koopa spawned.
    ACTOR_PARAM_CONFIG(BlockAppear, 28, 1); ///< Whether the Koopa should spawn from a block.
    ACTOR_PARAM_CONFIG(SpitOut, 29, 1); ///< Whether this Koopa was spat out by Yoshi.
};
