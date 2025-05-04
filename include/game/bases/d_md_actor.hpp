#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_sphere.hpp>

/// @brief The minimum required implementation for a cutscene actor.
/// @ingroup bases
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
    mSphere_c mCullSphere; ///< @brief A sphere representing the actor's visible area. @unused
};
