#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m3d/m3d.hpp>
#include <game/mLib/m_sphere.hpp>

/// @brief Base class for world map actors.
class dWmActor_c : public dBaseActor_c {
    dWmActor_c();
    ~dWmActor_c();

    virtual int preCreate();
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    virtual int GetActorType();

    /// @brief Creates a world map actor without a parent. See dBaseActor_c::construct.
    static dWmActor_c *construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);
    /// @brief Creates a child world map actor with the given parent. See dBaseActor_c::construct.
    static dWmActor_c *construct(ProfileName profName, dBase_c *base, unsigned long param, const mVec3_c *position, const mAng3_c *rotation);
    
    static void setSoftLight_Map(m3d::bmdl_c &mdl); ///< Sets the soft light effect for map actors.
    static void setSoftLight_MapObj(m3d::bmdl_c &mdl); ///< Sets the soft light effect for map objects.
    static void setSoftLight_Enemy(m3d::bmdl_c &mdl); ///< Sets the soft light effect for enemies.
    static void setSoftLight_Boss(m3d::bmdl_c &mdl); ///< Sets the soft light effect for bosses.

private:
    mSphere_c mCullSphere; ///< @brief A sphere representing the actor's visible area.
};
