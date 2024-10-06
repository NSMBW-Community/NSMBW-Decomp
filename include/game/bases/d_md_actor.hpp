#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_sphere.hpp>

/**
 * @brief The minimum required implementation for a cutscene actor.
 * @ingroup bases
 * @details
 * ## Overview
 * dMdActor_c serves as the base for all cutscene actors (Md stands for Movie Demo). It does not add any
 * particularly relevant components, but a few overloads are provided for convenience.
 *
 * ## Notable Operation Changes
 * The following operation changes have been made:
 * - The execute operation is not performed if the game @ref dGameCom::isGameStop "is stopped" for
 * @ref dGameCom::GAME_STOP_e::GAME_STOP_ANY "any reason".
 *
 * ## Unused Content
 * - The @ref mCullSphere "bounding sphere" mechanism from dWmActor_c is present in this class as well,
 * however no cutscene actors make use of it.
 * - Similarly, the World Select Menu check from the same class is also present, but it will never be
 * triggered since no cutscene actors are ever active on the world map.
 * - Due to the above, it is theorized that cutscene actors were likely intended to support more complex
 * behaviour similar to their world map counterparts, however this was scrapped in favour of a simpler
 * approach.
 */
class dMdActor_c : public dBaseActor_c {
    dMdActor_c(); ///< @copydoc dBaseActor_c::dBaseActor_c

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual ~dMdActor_c(); ///< @copydoc dBaseActor_c::~dBaseActor_c

    virtual int GetActorType();

    /// @brief Creates a child cutscene actor with the given parent. See
    /// @ref dBaseActor_c::construct(ProfileName, dBase_c*, unsigned long, const mVec3_c*, const mAng3_c*)
    /// "dBaseActor_c::construct".
    static dMdActor_c *construct(ProfileName profName, dBase_c *base, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);

private:
    mSphere_c mCullSphere; ///< @unused A sphere representing the actor's visible area.
};
