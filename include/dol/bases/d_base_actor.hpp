#pragma once
#include <types.h>
#include <dol/bases/d_base.hpp>
#include <dol/cLib/c_list.hpp>
#include <dol/framework/f_list_mg.hpp>
#include <dol/mLib/m_angle.hpp>
#include <dol/mLib/m_mtx.hpp>
#include <dol/mLib/m_vec.hpp>

/// @brief Basic actor class, supporting positioning, rotation and movement/acceleration.
/// @details All actors inheriting from this class use fProfile::fActorProfile_c for the profile
/// (instead of fProfile::fBaseProfile_c) and are children to the current scene actor.
class dBaseActor_c : public dBase_c {
public:

    /// @brief An identifier that represents the actor's kind.
    /// @details [This does not seem to be used outside of worldmaps].
    enum ACTOR_KIND_e {
        SYSTEM_ACTOR, ///< Used for behind-the-scenes actors [such as dCsSeqMng_c].
        MAP_DEMO_ACTOR, ///< Used for map actors that are affected by cutscenes.
        MAP_ACTOR, ///< Used for most map actors.
        MAP_ENEMY, ///< Used for map enemies.
        MAP_PLAYER, ///< The worldmap player actor [dWmPlayer_c].
        MAP_STOP, ///< The unused daWmStop_c actor.
        UNK_6, ///< [Unused].
        MENU_PLAYER, ///< The menu player actor [da2DPlayer_c].
        GENERIC_ACTOR, ///< The default value.
    };

    dBaseActor_c();

protected:
    virtual int preCreate(); ///< See dBase_c::preCreate.
    virtual void postCreate(fBase_c::MAIN_STATE_e status); ///< See dBase_c::postCreate.

    virtual int preDelete(); ///< See dBase_c::preDelete.
    virtual void postDelete(fBase_c::MAIN_STATE_e status); ///< See dBase_c::postDelete.

    /// @brief Code to be executed before ::execute.
    /// @details The actor's ::execute method is not called if the game is being reset.
    virtual int preExecute();

    /// @brief Code to be executed after ::execute.
    /// @details This method updates the actor's previous position and the position delta.
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    /// @brief Code to be executed before ::draw.
    /// @details The actor is not drawn if ::mVisible is @p false .
    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status); ///< See dBase_c::postDraw.

    virtual ~dBaseActor_c();

    /// @brief Alternate drawing function for drawing in front of 2D layouts (first draw pass).
    /// @details This feature is not used in stages.
    virtual void draw2D();

    /// @brief Alternate drawing function for drawing in front of 2D layouts (second draw pass).
    /// @details This feature is not used in stages.
    virtual void draw2D_lyt2();

    virtual int GetActorType(); ///< Gets the actor kind. See ::ACTOR_KIND_e.

    virtual void finalUpdate(); ///< Code to be executed after all the actors' ::execute method has run.

public:
    /// @brief Updates the actor's speed (3D actors).
    /// @details ::mSpeedF gets applied to the direction the actor is facing, while
    /// ::mAccelY is added to the vertical speed.
    void calcSpeed();

    /// @brief Updates the actor's speed (2D actors).
    /// @details ::mSpeedF gets applied to the X axis.
    void calcSpeedXY();

    void calcSpeedX(); ///< Applies ::mAccelF to the actor's X speed, using ::mSpeedMax as the speed limit.
    void calcFallSpeed(); ///< Applies ::mAccelY to the actor's Y speed, using ::mMaxFallSpeed as the speed limit.
    void calcSpeedY(); ///< Applies ::mAccelY to the actor's Y speed, using ::mSpeedMax as the speed limit.
    void calcSpeedF(); ///< Applies ::mAccelF to ::mSpeedF, using ::mMaxSpeedF as the speed limit.

    void makeMtx(); ///< Updates the actor's world matrix.

    mVec3_c getCenterPos() const; ///< Gets the actor's center position.
    void posMove(mVec3_c &delta); ///< Moves the actor by the given delta.
    void posMove(); ///< Moves the actor by its speed.

    int GetProfNameActorNum(ProfileName profile); ///< Counts the instances of the given actor profile.

    static void draw2DActorOnLyt1(); ///< Calls ::draw2D on every actor.
    static void draw2DActorOnLyt2(); ///< Calls ::draw2D_lyt2 on every actor.

    /**
     * @brief Creates an actor without a parent.
     *
     * @details The actor is created as a child of the current scene actor so that all actors
     * can be deleted on a scene change, working as a garbage collector of sorts.
     * @param profName The actor's profile name.
     * @param param The actor's parameters.
     * @param position The actor's position.
     * @param rotation The actor's rotation.
     * @return A pointer to the instantiated actor, or @p nullptr .
     */
    static dBaseActor_c *construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);

    /**
     * @brief Creates a child actor with the given parent.
     *
     * @param profName The actor's profile name.
     * @param parent The actor's parent.
     * @param param The actor's parameters.
     * @param position The actor's position.
     * @param rotation The actor's rotation.
     * @return A pointer to the instantiated actor, or @p nullptr .
     */
    static dBaseActor_c *construct(ProfileName profName, dBase_c *parent, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);

private:
    /**
     * @brief Sets temporary data to be used for the next actor's construction.
     *
     * @param position The actor's position.
     * @param rotation The actor's rotation.
     */
    static void setTmpCtData(const mVec3_c *position, const mAng3_c *rotation);

    fLiNdBa_c mLinkActor; ///< The node in ::m_actorManage.

public:
    mMtx_c mMatrix; ///< The actor's world matrix.

    mVec3_c mPos; ///< The actor's position (top-left corner).
    mVec3_c mLastPos; ///< The actor's position in the previous frame.
    mVec3_c mPosDelta; ///< The actor's position delta since the previous frame.

    mVec3_c mCenterOffs; ///< The offset from the position to the center of the actor (defaults to 0).
    mVec3_c mScale; ///< The actor's scale (defaults to 1).
    mVec3_c mSpeed; ///< The actor's speed.
    mVec3_c mSpeedMax; ///< The actor's maximum speed.

    mAng3_c mAngle; ///< The actor's rotation.
    mAng3_c mLastAngle; ///< The actor's rotation in the previous frame.

    float mSpeedF; ///< The actor's horizontal speed.
    float mMaxSpeedF; ///< The actor's maximum horizontal speed.
    float mAccelY; ///< The actor's vertical acceleration.
    float mMaxFallSpeed; ///< The actor's maximum fall speed.
    float mAccelF; ///< The actor's horizontal acceleration.

    u32 mActorProperties; ///< The actor's properties. See fProfile::fActorProfile_c::mActorProperties.
    bool mVisible; ///< Whether the actor should be visible or not. Defaults to @p true .

private:
    static mVec3_c *m_tmpCtPosP; ///< Temporary storage for the next constructed actor's position. See ::mPos.
    static mAng3_c *m_tmpCtAngleP; ///< Temporary storage for the next constructed actor's rotation. See ::mAngle.
    static fLiMgBa_c m_actorManage; ///< A list of all the constructed actors.
};
