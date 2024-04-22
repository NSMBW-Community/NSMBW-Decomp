#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_sphere.hpp>

/// @brief Base class for cutscene actors.
class dMdActor_c : public dBaseActor_c {
    dMdActor_c();
    ~dMdActor_c();

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual int GetActorType();

    static dMdActor_c *construct(ProfileName profName, dBase_c *base, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);

private:
    mSphere_c mCullSphere; ///< @brief A sphere representing the actor's visible area.
};
