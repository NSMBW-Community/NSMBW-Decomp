#pragma once
#include <types.h>
#include <game/bases/d_base.hpp>
#include <game/cLib/c_list.hpp>
#include <game/framework/f_list_mg.hpp>
#include <game/mLib/m_angle.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/mLib/m_vec.hpp>

/// @brief The minimum required implementation for an actor base.
/// @ingroup bases
class dBaseActor_c : public dBase_c {
public:

    /// @brief An identifier that represents the actor's kind.
    /// @details [This is only used in worldmaps].
    enum ACTOR_KIND_e {
        ACTOR_MAP_GENERIC, ///< A generic map actor (dWmActor_c).
        ACTOR_MAP_DEMO, ///< A map actor affected by cutscenes (dWmDemoActor_c).
        ACTOR_MAP_OBJECT, ///< A map object (dWmObjActor_c).
        ACTOR_MAP_ENEMY, ///< A map enemy (dWmEnemy_c).
        ACTOR_MAP_PLAYER, ///< The worldmap player actor (dWmPlayer_c).
        ACTOR_MAP_STOP, ///< The unused map stop actor (daWmStop_c). @unused
        ACTOR_UNK_6, ///< @unused
        ACTOR_MENU_PLAYER, ///< The menu player actor (da2DPlayer_c).
        ACTOR_GENERIC, ///< A generic non-map actor.
    };

    dBaseActor_c(); ///< Constructs a new actor.

protected:
    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual ~dBaseActor_c(); ///< Destroys the actor.

    /// @brief Alternate drawing function used to draw 3D models in front of 2D graphics (first draw pass).
    /// @details This feature is not used in stages. See [here](#drawing-models-over-2d-layouts)
    /// for details.
    virtual void draw2D();

    /// @brief Alternate drawing function used to draw 3D models in front of 2D graphics (second draw pass).
    /// @details This feature is not used in stages. See [here](#drawing-models-over-2d-layouts)
    /// for details.
    virtual void draw2D_lyt2();

public:
    virtual int GetActorType() { return ACTOR_GENERIC; } ///< Gets the actor kind. See ::ACTOR_KIND_e.

protected:
    virtual void finalUpdate() {} ///< Code to be executed after all actors' @p execute operation has run.

public:
    /// @brief Updates the actor's speed (3D actors). See [here](#speed-and-acceleration) for details.
    void calcSpeed();

    /// @brief Updates the actor's speed (2D actors). See [here](#speed-and-acceleration) for details.
    void calcSpeedXY();

    void calcSpeedX(); ///< Updates the actor's X speed. See [here](#speed-and-acceleration) for details.
    void calcFallSpeed(); ///< Updates the actor's falling speed. See [here](#speed-and-acceleration) for details.
    void calcSpeedY(); ///< Updates the actor's Y speed. See [here](#speed-and-acceleration) for details.
    void calcSpeedF(); ///< Updates the actor's forward speed. See [here](#speed-and-acceleration) for details.

    /// @brief Generates a partial transformation matrix for the actor and stores it in ::mMatrix.
    /// @details The transformation matrix only contains the translation and the rotation on the Y axis.
    void makeMtx();

    mVec3_c getCenterPos() const; ///< Gets the actor's centered position.
    void posMove(mVec3_c &delta); ///< Moves the actor by the given delta.
    void posMove(); ///< Moves the actor by its @ref mSpeed "speed".

    // [why is this not static?]
    int GetProfNameActorNum(ProfileName profile); ///< Counts the instances of the given actor profile.

    static void draw2DActorOnLyt1(); ///< Calls ::draw2D on every actor.
    static void draw2DActorOnLyt2(); ///< Calls ::draw2D_lyt2 on every actor.

    /**
     * @brief Creates an actor without a parent.
     *
     * @details The actor is created as a child of the current scene actor, so that all actors can be
     * deleted on a scene change, acting as a garbage collection mechanism.
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
     * @param parent The actor's parent. Must not be @p nullptr .
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
    mMtx_c mMatrix; ///< The actor's partial transformation matrix. See ::makeMtx for details.

    mVec3_c mPos; ///< The actor's position.
    mVec3_c mLastPos; ///< The actor's position in the previous frame.
    mVec3_c mPosDelta; ///< The actor's position delta since the previous frame.

    mVec3_c mCenterOffs; ///< The offset from the position to the center of the actor (defaults to 0).
    mVec3_c mScale; ///< The actor's scale (defaults to 1).
    mVec3_c mSpeed; ///< The actor's speed.
    mVec3_c mSpeedMax; ///< The actor's maximum speed.

    mAng3_c mAngle; ///< The actor's rotation (for 2D actors).
    mAng3_c mAngle3D; ///< The actor's rotation (for 3D actors).

    float mSpeedF; ///< The actor's horizontal speed.
    float mMaxSpeedF; ///< The actor's maximum horizontal speed.
    float mAccelY; ///< The actor's vertical acceleration.
    float mMaxFallSpeed; ///< The actor's maximum fall speed.
    float mAccelF; ///< The actor's horizontal acceleration.

    u32 mActorProperties; ///< The actor's properties. See fProfile::fActorProfile_c::mActorProperties.
    bool mVisible; ///< Whether the actor should be visible or not. Defaults to @p true .

private:
    static const mVec3_c *m_tmpCtPosP; ///< Temporary storage for the next constructed actor's position. See ::mPos.
    static const mAng3_c *m_tmpCtAngleP; ///< Temporary storage for the next constructed actor's rotation. See ::mAngle.
    static fLiMgBa_c m_actorManage; ///< A list of all actor bases.
};
