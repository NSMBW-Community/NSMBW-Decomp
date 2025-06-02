#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_rc.hpp>
#include <_dummy_classes.hpp>

class dAcPy_c;

/// @brief The minimum required implementation for a stage actor.
/// @ingroup bases
class dActor_c : public dBaseActor_c {
public:

    /// @brief The points awarded when the actor is eaten by Yoshi.
    enum EAT_POINTS_e {
        EAT_POINTS_200, ///< Awards 200 points (default).
        EAT_POINTS_1000, ///< Awards 1000 points.
        EAT_POINTS_NONE ///< Does not award points.
    };

    /// @brief The possible actor eat states.
    enum EAT_STATE_e {
        EAT_STATE_NONE, ///< The actor is not eaten.
        EAT_STATE_EATING, ///< The actor is about to be eaten.
        EAT_STATE_EATEN, ///< The actor was eaten.
        EAT_STATE_SPIT, ///< The actor is about to be spat out. Used for spitting out players when Yoshi is dismounted.
        EAT_STATE_SPAT ///< The actor was spat out.
    };

    /// @brief The possible actor eating behaviours.
    enum EAT_TYPE_e {
        EAT_TYPE_NONE, ///< The actor cannot be eaten.
        EAT_TYPE_EAT, ///< The actor can be eaten and then spat out.
        EAT_TYPE_EAT_PERMANENT, ///< The actor can be eaten permanently (default).
        EAT_TYPE_UNK3, ///< Unknown behaviour. Used on Fruits, Pokeys and Giant Fuzzies.
        EAT_TYPE_FIREBALL, ///< If the actor is eaten, Yoshi can then spit a fireball.
        EAT_TYPE_ICEBALL, ///< If the actor is eaten, Yoshi can then spit an iceball.
    };

    /// @brief The possible stage actor kinds.
    enum STAGE_ACTOR_KIND_e {
        STAGE_ACTOR_GENERIC, ///< A generic stage actor.
        STAGE_ACTOR_PLAYER, ///< The @ref dAcPy_c player actor.
        STAGE_ACTOR_YOSHI, ///< The @ref daYoshi_c Yoshi actor.
        STAGE_ACTOR_ENTITY, ///< An entity actor.
    };

    /// @brief The possible carry actions.
    enum CARRY_ACTION_e {
        CARRY_RELEASE = BIT_FLAG(0), ///< The actor is being released from carry.
        CARRY_THROW = BIT_FLAG(1), ///< The actor is being actively thrown by the player.
    };

    /// @brief The actor's checked collision directions.
    enum BG_COLL_FLAG_e {
        COLL_HEAD = BIT_FLAG(0), ///< The actor can collide with ceilings.
        COLL_WALL_L = BIT_FLAG(1), ///< The actor can collide with walls on its left.
        COLL_WALL_R = BIT_FLAG(2), ///< The actor can collide with walls on its right.
        COLL_FOOT = BIT_FLAG(3), ///< The actor can collide with the ground.
    };

    /// @brief Information related to actor spawning.
    enum ACTOR_SPAWN_FLAG_e {
        ACTOR_SPAWNED = BIT_FLAG(0), ///< The actor is spawned.
        ACTOR_NO_RESPAWN = BIT_FLAG(3), ///< The actor must not be respawned after deletion.
    };

    dActor_c(); ///< @copydoc dBaseActor_c::dBaseActor_c
    ~dActor_c(); ///< @copydoc dBaseActor_c::~dBaseActor_c

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual const char *getKindString() const;

    /// @brief Checks if the actor is out of view.
    /// @return @p true if the actor is out of view, else @p false.
    virtual bool ActorDrawCullCheck();

    virtual void block_hit_init(); ///< Callback for when a block below the actor is hit.

    virtual bool vf68(dBg_ctr_c *collider) { return true; } ///< Unknown, related to collision.
    virtual s8 *getPlrNo() { return &mPlayerNo; } ///< Gets the player number associated with the actor.
    virtual mVec2_c getLookatPos() const; ///< Gets the position players look to when focused on the actor.

    virtual bool isSpinLiftUpEnable() { return true; }
    virtual void setSpinLiftUpActor(dActor_c *actor);
    virtual void setCarryFall(dActor_c *actor, int collisionDelay) {}

    virtual void setEatTongue(dActor_c *actor);
    virtual void setEatTongueOff(dActor_c *actor);
    virtual void setEatMouth(dActor_c *actor);
    virtual bool setEatSpitOut(dActor_c *actor);
    virtual bool setEatGlupDown(dActor_c *actor);
    virtual void eatMove(dActor_c *actor);

    virtual void removeCc(); ///< Disables the actor's collision.
    virtual void reviveCc(); ///< Enables the actor's collision.
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

    /// @brief Generates a water splash effect.
    /// @param pos The effect position.
    /// @param size The effect scale.
    virtual void waterSplashEffect(const mVec3_c &pos, float size);

    /// @brief Generates a lava splash effect.
    /// @param pos The effect position.
    /// @param size The effect scale.
    virtual void yoganSplashEffect(const mVec3_c &pos, float size);

    /// @brief Generates a poison water splash effect.
    /// @param pos The effect position.
    /// @param size The effect scale.
    virtual void poisonSplashEffect(const mVec3_c &pos, float size);

    bool checkAreaNo(); ///< Checks if at least one player is in the same zone as the actor.

    static void setSoftLight_Player(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Player
    static void setSoftLight_Enemy(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Enemy
    static void setSoftLight_Map(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Map
    static void setSoftLight_MapObj(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_MapObj
    static void setSoftLight_Boss(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Boss
    static void setSoftLight_Item(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Item

    /// @brief Deletes the actor.
    /// @param deleteForever Whether the actor must not respawn after being deleted. Only applies to actors created by sprites.
    void deleteActor(u8 deleteForever);

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

    /// @brief Returns whether the actor is being carried by a player.
    /// @param playerNum A pointer where the carrying player's number will be written to, or @p nullptr .
    /// @return Whether the actor is being carried by a player.
    bool checkCarried(int *playerNum);

    /// @brief Gets the closest @ref dAcPy_c "player" to the actor and its distance.
    /// @details Centered positions are used to calculate the distance.
    /// @param delta The vector where the distance between the actor and the player will be written to.
    /// @return The closest player, or @p nullptr .
    dAcPy_c *searchNearPlayer(mVec2_c &delta);

    /// @brief Gets the closest @ref dAcPy_c "player" to the given position and its distance.
    /// @details Centered positions are used to calculate the distance.
    /// @param delta The vector where the distance to the player will be written to.
    /// @param pos The position to use for the search.
    /// @return The closest player, or @p nullptr .
    typedef dAcPy_c *(*searchNearPlayerFunc)(mVec2_c &delta, const mVec2_c &pos);
    static dAcPy_c *searchNearPlayer_Main(mVec2_c &delta, const mVec2_c &pos); ///< @see searchNearPlayerFunc
    static dAcPy_c *searchNearPlayerNormal(mVec2_c &delta, const mVec2_c &pos); ///< @see searchNearPlayerFunc
    static dAcPy_c *searchNearPlayerLoop(mVec2_c &delta, const mVec2_c &pos); ///< @see searchNearPlayerFunc

    /// @brief Gets the direction the target position is in, from the source position's viewpoint.
    /// @param trgX The target X position.
    /// @param srcX The source X position.
    /// @return The direction the target is in.
    typedef bool (*getTrgToSrcDirFunc)(float trgX, float srcX);
    static bool getTrgToSrcDir_Main(float trgX, float srcX); ///< @see getTrgToSrcDirFunc
    static bool getTrgToSrcDirNormal(float trgX, float srcX); ///< @see getTrgToSrcDirFunc
    static bool getTrgToSrcDirLoop(float trgX, float srcX); ///< @see getTrgToSrcDirFunc

    /// @brief Updates the given position to keep looping stages in mind.
    /// @param pos The position to be updated.
    /// @param ang The rotation (unused).
    /// @param param3 Always 1 (unused).
    static void changePosAngle(mVec3_c *pos, mAng3_c *ang, int param3);

    /// @brief Sets the player search function to be used for the level.
    /// @param loopType The loop type. See dScStage_c::LOOP_TYPE_e.
    static void setSearchNearPlayerFunc(int loopType);

    /// @brief Sets the direction detection function to be used for the level.
    /// @param loopType The loop type. See dScStage_c::LOOP_TYPE_e.
    static void setGetTrgToSrcDirFunc(int loopType);

    /// @brief Sets the position update function to be used for the level.
    /// @param loopType The loop type. See dScStage_c::LOOP_TYPE_e.
    static void setChangePosAngleFunc(int loopType);

    /// @brief Sets the position-related functions for the level.
    /// @param loopType The loop type. See dScStage_c::LOOP_TYPE_e.
    static void setLoopFunc(int loopType);

    void setKind(u8 kind); ///< Sets the actor's kind. See ::STAGE_ACTOR_KIND_e.

    /// @brief Sets temporary data to be used for the next actor's construction.
    /// @param layer The actor's layer.
    static void setTmpCtData(u8 layer);

    /**
     * @brief Creates a stage actor without a parent.
     *
     * @details The actor is created as a child of the current scene actor, so that all actors can be
     * deleted on a scene change, acting as a garbage collection mechanism.
     * @param profName The actor's profile name.
     * @param param The actor's parameters.
     * @param position The actor's position.
     * @param rotation The actor's rotation.
     * @param layer The actor's layer.
     * @return A pointer to the instantiated actor, or @p nullptr .
     */
    static dActor_c *construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer);

    /**
     * @brief Creates a child stage actor with the given parent.
     *
     * @param profName The actor's profile name.
     * @param parent The actor's parent. Must not be @p nullptr .
     * @param param The actor's parameters.
     * @param position The actor's position.
     * @param rotation The actor's rotation.
     * @param layer The actor's layer.
     * @return A pointer to the instantiated actor, or @p nullptr .
     */
    static dActor_c *construct(ProfileName profName, dBase_c *parent, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer);

    u8 m_00; ///< Seems to be a player bit flag. @unused
    u32 mCarryingPlayerNo; ///< The player number of the player carrying this actor. -1 if not carried.
    CARRY_ACTION_e mCarryingFlags; ///< The actor's carry actions.
    u32 mThrowDirection; ///< The actor's direction when thrown or dropped after carrying.
    u32 mPointsCombo; ///< The current points combo obtained by the actor by colliding with other actors.
    u8 m_13; ///< @unused
    u32 m_17; ///< @unused
    float m_1b; ///< @unused
    u32 m_1f; ///< @unused

    dCc_c mCc; ///< The actor's actor collision sensor.
    dBc_c mBc; ///< The actor's tile collision sensor.
    dRc_c mRc; ///< The actor's ride manager.

    mVec2_c m_1eb; ///< @todo Figure out the purpose of this field.

    mVec2_c mVisibleAreaSize;
    mVec2_c mVisibleAreaOffset;
    mBoundBox mMaxBound;
    mBoundBox mDestroyBound;
    u8 mDirection; ///< The actor's facing direction.
    u8 mAreaNo; ///< The actor's zone ID.
    u8 mBgCollFlags; ///< @copydoc BG_COLL_FLAG_e

    u8 *mpSpawnFlags; ///< The spawn flags for the actor. See ::ACTOR_SPAWN_FLAG_e.
    u16 *mpDeleteVal; ///< @unused
    u32 mEventNums; ///< The event IDs the actor is tracking.
    u64 mEventMask; ///< The event mask, generated from ::mEventNums.

    u32 m_23b;
    u16 mSpriteSpawnFlags; ///< The spawn flags from the sprite data entry.
    bool mBlockHit; ///< Whether a block below the actor was hit.

    u32 mEatenByID; ///< The @ref fBase_c::mUniqueID "unique identifier" of the eater actor.
    u8 mEatState; ///< The actor's eat state. Value is a ::EAT_STATE_e.
    u8 mEatBehaviour; ///< The actor's eat behaviour. Value is a ::EAT_TYPE_e.
    mVec3_c mPreEatScale; ///< The actor's scale before being eaten.

    EAT_POINTS_e mEatPoints; ///< @copydoc EAT_POINTS_e
    u32 m_25b;
    u32 m_25f;

    void *mCarryPropelActor;
    u8 mKind; ///< The actor's kind. Value is a ::STAGE_ACTOR_KIND_e.
    s8 mPlayerNo; ///< The player associated with the actor, -1 if not associated to any player.
    u8 mExecStopMask;
    u8 mLayer; ///< The actor's layer.
    bool mNoRespawn; ///< Whether the actor should not respawn after being deleted.
    bool mBackFence; ///< Whether the actor is on the back side of chainlink fences.

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

    static u8 mExecStopReq; ///< The actor kinds for which the @p execute operation is requested to be disabled.
    static u8 mDrawStopReq; ///< The actor kinds for which the @p draw operation is requested to be disabled.
    static u8 mExecStop; ///< The actor kinds for which the @p execute operation is disabled.
    static u8 mDrawStop; ///< The actor kinds for which the @p draw operation is disabled.
    static searchNearPlayerFunc mSearchNearPlayerFunc; ///< The player search function.
    static getTrgToSrcDirFunc mGetTrgToSrcDirFunc; ///< The direction detection function.
    static u8 m_tmpCtLayerNo; ///< Temporary storage for the next constructed actor's layer. See ::mLayer.

    /// @brief Temporary storage for the next created sprite actor's spawn flags. See ::mpSpawnFlags. @unofficial
    static u8* m_tmpCtSpawnFlags;

    /// @brief Temporary storage for the next created sprite actor's tracked event IDs. See ::mEventNums. @unofficial
    static u16 m_tmpCtEventNums;

    /// @brief Temporary storage for the next created sprite actor's event mask. See ::mEventMask. @unofficial
    static u64 m_tmpCtEventMask;

    /// @brief Temporary storage for the next created sprite actor's layer. See ::mLayer. @unofficial
    static u8 m_tmpCtSpriteLayerNo;
};

extern const u8 l_Ami_Line[2]; ///< The chainlink mode for each side of chainlink fences.
extern const float l_Ami_Zpos[2]; ///< The additional Z offset for each side of chainlink fences.
