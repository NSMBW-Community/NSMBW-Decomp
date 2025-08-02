#pragma once

#include <game/bases/d_base_actor.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_sphere.hpp>

/// @brief The minimum required implementation for a world map actor.
/// @ingroup bases
class dWmActor_c : public dBaseActor_c {
public:
    dWmActor_c(); ///< @copydoc dBaseActor_c::dBaseActor_c

protected:

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
