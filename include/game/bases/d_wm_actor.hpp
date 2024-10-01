#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m3d/m3d.hpp>
#include <game/mLib/m_sphere.hpp>
/**
 * @brief The minimum required implementation for a world map actor.
 * @ingroup bases
 * @details
 * ## Overview
 * dWmActor_c serves as the base for all map actors. It is typically not used directly, unless the actor's sole purpose
 * is for background tasks or unconditional 3D model rendering. For more complex behaviors, it is recommended to use
 * dWmDemoActor_c and its sub-implementations, which enable the actor to respond to world map events and more.
 *
 * ## Creating Map Actors
 * Map actors can be created by calling one of the two ::construct methods, which define two
 * additional arguments to provide the starting position and rotation. A parent base can optionally
 * be provided.
 *
 * ## Notable Operation Changes
 * The following operation changes have been made:
 * - The execute operation is not performed if the game @ref dGameCom::isGameStop "is stopped" for
 * @ref dGameCom::GAME_STOP_e::GAME_STOP_ANY "any reason".
 * - The draw operation is not performed if the actor is outside the camera view. See [here](#bounding-sphere)
 * for details.
 * - The execute and draw operations are also skipped if the World Select Menu is open.
 *  - This is done to ensure the blurring effect behind the menu works as intended: the game captures a screenshot of
 * the map, displaying it instead of the active world, while all animations are paused to avoid abrupt transitions upon
 * exiting the menu.
 *  - @p WM_CS_SEQ_MNG , @p WM_ISLAND and @p WM_DIRECTOR are the only actors allowed to keep running while the menu is
 * open.
 *
 * ## Bounding Sphere
 * The game uses a @ref mCullSphere "bounding sphere" to perform
 * @xlink{https://bruop.github.io/frustum_culling/,frustum culling} on the actor.
 *
 * If the actor's bounding sphere is not within the camera view, it will not be rendered. This behaviour can
 * be controlled by setting the sphere's @ref mSphere_c::mRadius "radius" (which must be done manually).
 * If the radius is set to @p 0.0f (the default value), the check is bypassed and the actor is always
 * drawn. The sphere's @ref mSphere_c::mPos "position" is updated automatically.
 *
 * @hint{It is recommended to set the sphere radius in the ::create method.}
 *
 * ## Lighting Adjustments
 * Wrappers to adjust the model lighting are provided for convenience. See dGameCom for details.
 *
 * @todo Use proper class names for the mentioned actors once decompiled.
 */
class dWmActor_c : public dBaseActor_c {
    dWmActor_c(); ///< @copydoc dBaseActor_c::dBaseActor_c

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual ~dWmActor_c(); ///< @copydoc dBaseActor_c::~dBaseActor_c

    virtual int GetActorType();

    /// @brief Creates a world map actor without a parent. See dBaseActor_c::construct.
    static dWmActor_c *construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);

    /// @brief Creates a child world map actor with the given parent. See
    /// @ref dBaseActor_c::construct(ProfileName, dBase_c*, unsigned long, const mVec3_c*, const mAng3_c*)
    /// "dBaseActor_c::construct".
    static dWmActor_c *construct(ProfileName profName, dBase_c *base, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);

    static void setSoftLight_Map(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Map
    static void setSoftLight_MapObj(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_MapObj
    static void setSoftLight_Enemy(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Enemy
    static void setSoftLight_Boss(m3d::bmdl_c &mdl); ///< @copydoc dGameCom::SetSoftLight_Boss

private:
    mSphere_c mCullSphere; ///< @brief A sphere representing the actor's visible area.
};
