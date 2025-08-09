#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_rc.hpp>

class dBg_ctr_c;

class dAcPy_c;
class dPropelParts_c;

/// @brief The minimum required implementation for a stage actor.
/// @ingroup bases
class dActor_c : public dBaseActor_c {
public:

    /// @brief The score awarded when the actor is eaten by Yoshi.
    enum EAT_POINTS_e {
        EAT_POINTS_200, ///< Awards 200 points (default).
        EAT_POINTS_1000, ///< Awards 1000 points. @unused
        EAT_POINTS_NONE ///< No points are awarded. @unused
    };

    /// @brief The possible actor eat states.
    enum EAT_STATE_e {
        EAT_STATE_NONE, ///< The actor is not being eaten.
        EAT_STATE_EATING, ///< The actor is in the process of being eaten.
        EAT_STATE_EATEN, ///< The actor has been successfully eaten.
        EAT_STATE_SPIT, ///< The actor is about to be spat out. Used for spitting out players when Yoshi is dismounted.
        EAT_STATE_SPAT ///< The actor has been spat out.
    };

    /// @brief The possible actor behaviors when eaten by Yoshi.
    enum EAT_BEHAVIOR_e {
        EAT_TYPE_NONE, ///< The actor cannot be eaten.
        EAT_TYPE_EAT, ///< The actor can be eaten and then spat out.
        EAT_TYPE_EAT_PERMANENT, ///< The actor is consumed permanently after being eaten (default).
        EAT_TYPE_UNK3, ///< Unknown behaviour. Used on Fruits, Pokeys and Giant Fuzzies.
        EAT_TYPE_FIREBALL, ///< Yoshi can spit a fireball after eating the actor.
        EAT_TYPE_ICEBALL, ///< Yoshi can spit an iceball after eating the actor.
    };

    /// @brief The possible stage actor kinds.
    enum STAGE_ACTOR_KIND_e {
        STAGE_ACTOR_GENERIC, ///< A generic stage actor (default).
        STAGE_ACTOR_PLAYER, ///< The @ref dAcPy_c "player actor".
        STAGE_ACTOR_YOSHI, ///< The @ref daYoshi_c "Yoshi actor".
        STAGE_ACTOR_ENTITY, ///< An interactable entity actor.
    };

    /// @brief The possible carry actions.
    enum CARRY_ACTION_e {
        CARRY_RELEASE = BIT_FLAG(0), ///< The actor is being released from carry.
        CARRY_THROW = BIT_FLAG(1), ///< The actor is being actively thrown by the player.
    };

    /// @brief The collision directions that an actor can respond to.
    enum BG_COLL_FLAG_e {
        COLL_NONE = BIT_FLAG(-1), ///< The actor does not collide with any surface.
        COLL_HEAD = BIT_FLAG(0), ///< The actor can collide with ceilings.
        COLL_WALL_L = BIT_FLAG(1), ///< The actor can collide with walls on its left.
        COLL_WALL_R = BIT_FLAG(2), ///< The actor can collide with walls on its right.
        COLL_FOOT = BIT_FLAG(3), ///< The actor can collide with the ground.
    };

    /// @brief Information related to actor spawning.
    enum ACTOR_SPAWN_FLAG_e {
        ACTOR_SPAWNED = BIT_FLAG(0), ///< The actor is spawned.
        ACTOR_NO_RESPAWN = BIT_FLAG(3), ///< The actor must not respawn after deletion.
    };

    /// @brief Flags used to control out of screen checks.
    enum SCREEN_OUT_e {
        SKIP_NONE = BIT_FLAG(-1), ///< No checks are skipped.
        SKIP_ACTOR_DELETE = BIT_FLAG(1), ///< The actor is not deleted if out of screen.
        SKIP_SCREEN_CHECK = BIT_FLAG(2), ///< The actor position is not checked against the screen boundaries.
        SKIP_RIDE_CHECK = BIT_FLAG(3), ///< The actor's ride status is not checked.
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

    virtual void block_hit_init(); ///< Callback for when a block directly beneath the actor is hit.

    virtual bool vf68(dBg_ctr_c *collider) { return true; } ///< Unknown, related to collision. @unofficial
    virtual s8 *getPlrNo() { return &mPlayerNo; } ///< Gets the player number associated with the actor. See ::mPlayerNo.
    virtual mVec2_c getLookatPos() const; ///< Gets the position players look to when focused on the actor.

    /// @brief Returns whether the actor can be carried.
    virtual bool isSpinLiftUpEnable() { return true; }

    /// @brief Callback for when the actor is picked up by another actor.
    /// @param carryingActor The actor performing the carrying action.
    virtual void setSpinLiftUpActor(dActor_c *carryingActor);

    /// @brief Callback for when the actor is dropped by the carrying actor.
    /// @param carryingActor The carrying actor.
    /// @param collisionDelay The amount of frames during which collisions with the former carrier are ignored.
    virtual void setCarryFall(dActor_c *carryingActor, int collisionDelay) {}

    /// @brief Callback for when the actor is targeted by Yoshi's tongue.
    /// @param eatingActor The actor performing the eating action.
    virtual void setEatTongue(dActor_c *eatingActor);

    /// @brief Callback for when the eating action is canceled.
    /// @details This only seems to occur when two actors try to eat the same actor at the same time.
    /// @param eatingActor The actor performing the eating action.
    virtual void setEatTongueOff(dActor_c *eatingActor);

    /// @brief Callback for when the actor is being eaten.
    /// @param eatingActor The actor performing the eating action.
    virtual void setEatMouth(dActor_c *eatingActor);

    /// @brief Callback for when the actor is about to be spat out.
    /// @param eatingActor The actor performing the eating action.
    /// @return Whether the actor should be spat out.
    virtual bool setEatSpitOut(dActor_c *eatingActor);

    /// @brief Callback for when the actor is about to be fully swallowed.
    /// @param eatingActor The eating actor.
    /// @return Always returns @p true .
    virtual bool setEatGlupDown(dActor_c *eatingActor);

    /// @brief Updates the actor's position during eating actions.
    /// @param eatingActor The eating actor.
    virtual void eatMove(dActor_c *eatingActor);

    virtual void removeCc(); ///< Disables the actor's collision.
    virtual void reviveCc(); ///< Enables the actor's collision.
    virtual void setAfterEatScale(); ///< Restores the actor's scale once spat out.
    virtual void calcSpitOutPos(dActor_c *eatingActor); ///< Calculates the position where the actor will be spat out.
    virtual float calcEatScaleRate(dActor_c *eatingActor); ///< Computes the scaling rate during eating.
    virtual void calcEatInScale(dActor_c *eatingActor); ///< Adjusts the actor's scale while being eaten.

    /// @brief Spawns the visual effects for when all players reach the flagpole and all enemies are cleared.
    virtual void allEnemyDeathEffSet();

    virtual void vfb4(); ///< @unofficial
    virtual void funsuiMoveX() {} ///< @unused
    virtual void cancelFunsuiActUpper(); ///< @unused
    virtual void cancelFunsuiActSide(int); ///< @unused
    virtual void cancelFunsuiActVanish(); ///< @unused

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

    /// @brief Deletes the actor and optionally disables respawn.
    /// @param deleteForever Whether the actor must not respawn after being deleted. Only applies to actors created by sprites.
    void deleteActor(u8 deleteForever);

    /// @brief Checks if the actor should be culled due to zone limits.
    /// @unofficial
    /// @param pos The actor position.
    /// @param bound The actor's bounding box.
    /// @param areaID The actor's zone ID.
    /// @return Whether the actor should be culled.
    bool areaCullCheck(const mVec3_c &pos, const mBoundBox &bound, u8 areaID) const;

    /// @brief Checks if the actor is out of gameplay and optionally deletes it.
    /// @param flags The flags to control which actions to perform. Value is a ::SCREEN_OUT_e.
    /// @return Whether the actor is out of gameplay.
    bool ActorScrOutCheck(u16 flags);

    /// @brief Checks if the actor should be culled due to being outside the screen.
    /// @decompnote{Likely belongs to the dGameCom namespace.}
    /// @unofficial
    /// @param pos The actor position.
    /// @param visibleBound The actor's visible bounding box.
    /// @param destroyBound The actor's deletion bounding box.
    /// @param areaID The actor's zone ID (unused).
    /// @return Whether the actor should be culled.
    static bool screenCullCheck(const mVec3_c &pos, const mBoundBox &visibleBound, mBoundBox destroyBound, u8 areaID);

    /// @brief Returns whether the actor is colliding with any enabled collision sides.
    /// @unofficial
    bool checkBgColl();

    /// @brief Checks if the prompt for the given action should be displayed for each player.
    /// @param fukidashiAction The action to be checked.
    /// @param fukidashiTriggerSize The size of the area around the actor where the prompt is displayed.
    /// @return Always returns @p false .
    bool carryFukidashiCheck(int fukidashiAction, mVec2_c fukidashiTriggerSize);

    /// @brief Searches the closest player who has not yet performed the given action.
    /// @param fukidashiAction The action to be checked.
    /// @return The closest player meeting the criteria, or @p nullptr if not found.
    dAcPy_c *searchCarryFukidashiPlayer(int fukidashiAction);

    /// @brief Hides the given action prompt for all players.
    /// @param fukidashiAction The action to be hidden.
    /// @param playerId The player for whom the action prompt should never be shown again, or @p -1 if none.
    void carryFukidashiCancel(int fukidashiAction, int playerId);

    /// @brief Kills the actor and optionally awards points to one or all players.
    /// @unofficial
    /// @param playerId The player to assign the score resulting from the kill, or @p -1 to award it to all players.
    /// @param noScore Whether points should not be awarded from the kill.
    void killActor(s8 playerId, int noScore);

    /// @brief Plays the combo kill sound effect.
    /// @param multiplier The combo multiplier.
    /// @param shortCombo Whether the combo starts at 1000 points instead of 200 points.
    void slideComboSE(int multiplier, bool shortCombo);

    void clrComboCnt(); ///< Clears the actor's combo counter.

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
    static dAcPy_c *searchNearPlayer_Main(mVec2_c &delta, const mVec2_c &pos); ///< See ::searchNearPlayerFunc.
    static dAcPy_c *searchNearPlayerNormal(mVec2_c &delta, const mVec2_c &pos); ///< See ::searchNearPlayerFunc.
    static dAcPy_c *searchNearPlayerLoop(mVec2_c &delta, const mVec2_c &pos); ///< See ::searchNearPlayerFunc.

    /// @brief Gets the direction the target position is in, from the source position's viewpoint.
    /// @param trgX The target X position.
    /// @param srcX The source X position.
    /// @return The direction the target is in.
    typedef bool (*getTrgToSrcDirFunc)(float trgX, float srcX);
    static bool getTrgToSrcDir_Main(float trgX, float srcX); ///< See ::getTrgToSrcDirFunc.
    static bool getTrgToSrcDirNormal(float trgX, float srcX); ///< See ::getTrgToSrcDirFunc.
    static bool getTrgToSrcDirLoop(float trgX, float srcX); ///< See ::getTrgToSrcDirFunc.

    /// @brief Adjusts the actor's position to account for looping stages.
    /// @param pos The position to be updated.
    /// @param ang The rotation (unused).
    /// @param param3 Always @p 1 (unused).
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
    u32 mCarryFukidashiPlayerNo; ///< The player for whom an action prompt related to the actor is being displayed. @p -1 if no players meet this criteria.
    CARRY_ACTION_e mCarryingFlags; ///< The actor's carry actions.
    u32 mThrowDirection; ///< The actor's direction when thrown or dropped after carrying.
    u32 mComboMultiplier; ///< The current combo multiplier obtained by the actor by colliding with other actors.
    u8 m_13; ///< @unused
    u32 m_17; ///< @unused
    float m_1b; ///< @unused
    u32 m_1f; ///< @unused

    dCc_c mCc; ///< The actor-to-actor collision sensor.
    dBc_c mBc; ///< The actor-to-tile collision sensor.
    dRc_c mRc; ///< The actor's ride surface manager.

    mVec2_c m_1eb; ///< @todo Figure out the purpose of this field.

    mVec2_c mVisibleAreaSize; ///< The size of the area inside which the actor is visible.
    mVec2_c mVisibleAreaOffset; ///< The offset applied to the area size.
    mBoundBox mMaxBound; ///< @todo Figure out the exact purpose of this field.
    mBoundBox mDestroyBound; ///< @todo Figure out the exact purpose of this field.
    u8 mDirection; ///< The actor's facing direction.
    u8 mAreaNo; ///< The actor's zone ID.
    u8 mBgCollFlags; ///< The collision directions that the actor can respond to.

    u8 *mpSpawnFlags; ///< The spawn flags for the actor. See ::ACTOR_SPAWN_FLAG_e.
    u16 *mpDeleteVal; ///< @unused
    u16 mEventNums; ///< The event IDs the actor is tracking.
    u64 mEventMask; ///< The event mask, generated from ::mEventNums.

    u32 m_23b; ///< @todo Figure out the purpose of this field.
    u16 mSpriteSpawnFlags; ///< The spawn flags from the sprite data entry.
    bool mBlockHit; ///< Whether a block below the actor was hit.

    u32 mEatenByID; ///< The @ref fBase_c::mUniqueID "unique identifier" of the eating actor.
    u8 mEatState; ///< The actor's eat state. Value is a ::EAT_STATE_e.
    u8 mEatBehaviour; ///< The actor's eat behaviour. Value is a ::EAT_BEHAVIOR_e.
    mVec3_c mPreEatScale; ///< The actor's scale before being eaten.

    EAT_POINTS_e mEatPoints; ///< @copydoc EAT_POINTS_e
    int mAttentionMode; ///< @todo Document this field and its values.
    u32 mAttentionFlags; ///< @todo Document this field and its values.

    dPropelParts_c *mPropelParts; ///< The actor's propeller effect manager.
    u8 mKind; ///< The actor's kind. Value is a ::STAGE_ACTOR_KIND_e.
    s8 mPlayerNo; ///< The player associated with the actor, @p -1 if not associated to any player.
    u8 mExecStopMask; ///< The mask required to disable the @p execute operation for the actor.
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

    static const float smc_CULL_XLIMIT; ///< The default @ref mMaxBound "max bound" X offset.
    static const float smc_CULL_YLIMIT; ///< The default @ref mMaxBound "max bound" Y offset.
    static const float smc_CULL_AREA_XLIMIT; ///< The default @ref mMaxBound "max bound" X size.
    static const float smc_CULL_AREA_YLIMIT; ///< The default @ref mMaxBound "max bound" Y size.
    static const mVec2_c smc_FUKIDASHI_RANGE; ///< The default trigger area size for displaying action prompts.

    static u8 mExecStopReq; ///< The actors for which the @p execute operation is requested to be disabled.
    static u8 mDrawStopReq; ///< The actor kinds for which the @p draw operation is requested to be disabled.
    static u8 mExecStop; ///< The actors for which the @p execute operation is currently disabled.
    static u8 mDrawStop; ///< The actor kinds for which the @p draw operation is currently disabled.
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

extern const u8 l_Ami_Line[2]; ///< The sub-layer for each side of chainlink fences.
extern const float l_Ami_Zpos[2]; ///< The additional Z offset for each side of chainlink fences.
