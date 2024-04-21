#pragma once
#include <types.h>
#include <game/bases/d_base.hpp>
#include <game/cLib/c_list.hpp>
#include <game/framework/f_list_mg.hpp>
#include <game/mLib/m_angle.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/mLib/m_vec.hpp>

/**
 * @brief The minimum required implementation for an actor base.
 * @ingroup bases
 * @details dBaseActor_c allows placing bases in either 2D or 3D space, by implementing the basic
 * concepts of translation, scale, rotation and visibility. It also comes with a barebones physics
 * engine and a few other properties. All actor bases (including sub-implementations) use
 * @ref fProfile::fActorProfile_c "fActorProfile_c" for their profile, and their instances are
 * added to a @ref ::m_actorManage "dedicated list" in addition to the default ones.
 *
 * The class is used directly when the only purpose of the base is displaying a 3D model.
 * For more complex behaviour, it's recommended to use one of the available sub-implementations:
 * - dActor_c, the minimum required implementation for a stage actor.
 * - dWmActor_c, the minimum required implementation for a worldmap actor.
 * - dMdActor_c, the minimum required implementation for a cutscene actor.
 *
 * ## Creating Actors
 * Actor bases can be created by calling one of the two ::construct methods, which define two
 * additional arguments to provide the starting position and rotation. A parent base can optionally
 * be provided.
 *
 * @hint{If using a sub-implementation, using the provided method overloads is recommended.}
 *
 * ## Notable Operation Changes
 * The following operation changes have been made:
 * - The @p execute operation will not run if the game is undergoing a @ref dReset::Manage_c "reset procedure".
 * - The @p finalUpdate step has been added to the @p execute operation. See below for more details.
 *
 * ### finalUpdate
 * @p finalUpdate is an additional step that runs after every actor's main @p execute operation steps have
 * been completed. It's only used in stages, and appears to have been introduced to address some model
 * positioning issues by delaying the execution of the related code.
 *
 * @hint{If an actor's model is being unwantedly repositioned, moving its model updating code to this
 * function may resolve the issue, particularly if the actor exhibits any of the following behaviours:
 * - The actor can be carried (e.g. POW Block, Barrel);
 * - The actor can be frozen and carried (e.g. Goomba, Spiny);
 * - The actor is thrown by an actor that can be frozen and carried (e.g. Hammer, Boomerang).}
 *
 * It is currently unknown why this fix is necessary, but incorrect base execution order is a likely
 * suspect.
 *
 * ## Positioning
 * Actor positions are stored as @ref mVec3_c "3-dimensional floating point vectors". The @ref mPos
 * "current position" can be modified manually, while the @ref mLastPos "previous position" and the
 * @ref mPosDelta "position delta" are updated automatically after the @p execute operation.
 *
 * @hint{The game uses a
 * @xlink{https://learn.microsoft.com/en-us/windows/uwp/graphics-concepts/coordinate-systems,
 * right-handed coordinate system}, with the Y axis used as height and the Z axis used as depth.}
 *
 * ### Notes for Stage Actors
 * - The position vector generally does not represent the center of the actor. To get the
 * centered position, use the ::getCenterPos function.
 * - In stages, the origin point is placed in the top left corner, therefore Y values must be negated.
 * - Tiles have a length and height of @p 16.0 .
 *
 * ## Moving an Actor
 * You can move an actor by calling the ::posMove function. A translation vector can optionally be
 * provided, otherwise the actor will be moved by its @ref mSpeed "current speed".
 *
 * @hint{Many subclasses will call this function automatically. Make sure to research this to avoid
 * unexpected behaviour!}
 *
 * ## Speed and Acceleration
 * Each axis has a dedicated speed value, which can be used to move the actor by a certain distance in
 * its direction. Acceleration can be applied linearly to the X/Y axes (and Z in a limited fashion) to
 * increase/decrease the actor's speed in said direction. More complex acceleration curves can be
 * utilized by modifying the acceleration value through other libraries.
 *
 * Speed on the X/Y axes can also be limited. However, the limits act as a target speed rather than a
 * hard cap: should the current speed on a given axis exceed the corresponding limit, the actor will
 * linearly decelerate until the limit is reached. Numerous functions are provided to update speed
 * values, and their behaviour is summarised into the table below:
 * <table>
 * <thead>
 *   <tr>
 *     <th>Function</th>
 *     <th>Supports</th>
 *     <th colspan="2">Speed Value Source(s)</th>
 *     <th>Speed Value Destination(s)</th>
 *     <th>Acceleration(s) Applied</th>
 *     <th>Speed Limit(s) Checked</th>
 *     <th>Usage Notes</th>
 *   </tr>
 * </thead>
 * <tbody>
 *   <tr>
 *     <td>::calcSpeedF</td>
 *     <td>All Actors</td>
 *     <td>**F**</td>
 *     <td>::mSpeedF</td>
 *     <td>::mSpeedF</td>
 *     <td>::mAccelF</td>
 *     <td>::mMaxSpeedF</td>
 *     <td>
 *      - This function cannot be used on its own to update horizontal speed, as the result is not
 *        placed into the ::mSpeed vector.
 *      - It is unknown what "F" stands for. Guesses are either Facing or Forward.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td rowspan="3">::calcSpeed</td>
 *     <td rowspan="3">3D Actors</td>
 *     <td>**X**</td>
 *     <td>::mSpeedF</td>
 *     <td>@ref mSpeed "mSpeed.x"</td>
 *     <td>*none*</td>
 *     <td>*none*</td>
 *     <td rowspan="3">
 *      - ::mSpeedF is applied into the direction the actor is facing.
 *      - Combine this with ::calcSpeedF to implement acceleration and limiting on the X/Z axes.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>**Y**</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>::mAccelY</td>
 *     <td>::mMaxFallSpeed</td>
 *   </tr>
 *   <tr>
 *     <td>**Z**</td>
 *     <td>::mSpeedF</td>
 *     <td>@ref mSpeed "mSpeed.z"</td>
 *     <td>*none*</td>
 *     <td>*none*</td>
 *   </tr>
 *   <tr>
 *     <td>::calcSpeedX</td>
 *     <td>2D Actors</td>
 *     <td>**X**</td>
 *     <td>@ref mSpeed "mSpeed.x"</td>
 *     <td>@ref mSpeed "mSpeed.x"</td>
 *     <td>::mAccelF</td>
 *     <td>@ref mSpeedMax "mSpeedMax.x"</td>
 *     <td></td>
 *   </tr>
 *   <tr>
 *     <td>::calcSpeedY</td>
 *     <td>2D Actors</td>
 *     <td>**Y**</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>::mAccelY</td>
 *     <td>@ref mSpeedMax "mSpeedMax.y"</td>
 *     <td>
 *      - This function is intended to be used with negative speed values, but will also work with
 *        positive ones.
 *      - Unlike all the other functions, this one treats the provided limit as a hard cap, so ensure it
 *        is used properly to prevent unintentional breakage.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>::calcFallSpeed</td>
 *     <td>2D Actors</td>
 *     <td>**Y**</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>::mAccelY</td>
 *     <td>::mMaxFallSpeed</td>
 *     <td>In order for this function to work correctly, the acceleration value must be negated (so
 *         downwards acceleration is achieved with positive values and viceversa).</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="2">::calcSpeedXY</td>
 *     <td rowspan="2">2D Actors</td>
 *     <td>**X**</td>
 *     <td>::mSpeedF</td>
 *     <td>@ref mSpeed "mSpeed.x"</td>
 *     <td>::mAccelF</td>
 *     <td>::mMaxSpeedF</td>
 *     <td rowspan="2">This function combines ::calcSpeedF (with the correct destination value) and
 *         ::calcFallSpeed, so make sure to read the notes for each of the two functions to ensure
 *         proper functionality.
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>**Y**</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>@ref mSpeed "mSpeed.y"</td>
 *     <td>::mAccelY</td>
 *     <td>::mMaxFallSpeed</td>
 *   </tr>
 * </tbody>
 * </table>
 *
 * ## Scaling
 * An actor base' scale can be controlled through the ::mScale vector. Depending on the base's
 * implementation, the effects may only be visual or there may be no change at all.
 *
 * ## Rotation
 * Actor rotations are stored as @ref mAng3_c "3-dimensional short vectors". There are two vectors:
 * ::mAngle, which is used by 2D actors, and ::mAngle3D, which is used by 3D actors. A small number of
 * bases ignore these conventions and employ the unused vector either as a backup or as a delta value.
 *
 * Just like for scaling, the effects of a rotation change also depend on the base's implementation.
 *
 * ## Visibility
 * The actor's visibility can be controlled through the ::mVisible attribute. The actor won't be drawn
 * if said property is set to @p false .
 *
 * ## Actor Properties
 * ::mActorProperties is a bitfield storing various properties, which are later interpreted by inheriting
 * classes. They are copied from the @ref fProfile::fActorProfile_c::mActorProperties
 * "corresponding field" in @ref fProfile::fActorProfile_c "fActorProfile_c" when the base is
 * constructed.
 *
 * ## Specialties
 * ### Drawing Models Over 2D Layouts
 * The ::draw2D function can replace the @p draw operation to allow 3D models to be drawn over the
 * game's interface (the ::mVisible attribute in this case is ignored). This feature can only be
 * used in the following scenes:
 * - Main Menu (dScGameSetup_c)
 * - Multiplayer Level Select (dScMulti_c)
 * - Multiplayer Results (dScResult_c)
 * - World 9 Unlock Cutscene (dScWorld9DeMo_c)
 * - World Map (dScWMap_c)
 *
 * The world map scene also supports an additional function (::draw2D_lyt2), which is executed in a
 * later draw pass.
 *
 * @hint{To use this feature in other scenes, simply call ::draw2DActorOnLyt1 (and optionally
 * ::draw2DActorOnLyt2) from the scene's paint function (do so before the LinkScene call for the
 * respective draw pass).}
 *
 * ### Profile Counting
 * The number of actor bases using a given profile can be obtained by calling ::GetProfNameActorNum.
 *
 * ### Map Actor Types
 * The ::GetActorType function offers basic type information about map actors. See ::ACTOR_KIND_e for
 * details.
 *
 * ## Unused Content
 * - The Z component of ::mSpeedMax is never used, despite being set to zero by various actors.
 * - ::ACTOR_KIND_e type @ref ACTOR_KIND_e::ACTOR_UNK_6 "UNK_6" is not used anywhere, and presumably
 * identifies a now-deleted actor.
 *
 * @todo Add missing links (especially the dGraph bit to avoid confusing terminology). Find the official
 * name of LinkScene and link it.
 */
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
        ACTOR_MAP_STOP, ///< @unused The unused map stop actor (daWmStop_c).
        ACTOR_UNK_6, ///< @unused Not used anywhere.
        ACTOR_MENU_PLAYER, ///< The menu player actor (da2DPlayer_c).
        ACTOR_GENERIC, ///< A generic non-map actor.
    };

    dBaseActor_c(); ///< @copydoc fBase_c::fBase_c

protected:
    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual ~dBaseActor_c(); ///< @copydoc fBase_c::~fBase_c

    /// @brief Alternate drawing function used to draw 3D models in front of 2D graphics (first draw pass).
    /// @details This feature is not used in stages. See [here](#drawing-models-over-2d-layouts)
    /// for details.
    virtual void draw2D();

    /// @brief Alternate drawing function used to draw 3D models in front of 2D graphics (second draw pass).
    /// @details This feature is not used in stages. See [here](#drawing-models-over-2d-layouts)
    /// for details.
    virtual void draw2D_lyt2();

    virtual int GetActorType(); ///< Gets the actor kind. See ::ACTOR_KIND_e.

    virtual void finalUpdate(); ///< Code to be executed after all actors' @p execute operation has run.

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
