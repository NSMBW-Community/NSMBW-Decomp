#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/smdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/// @brief The actor for the Warp Cannons found on the World Map.
/// @ingroup bases
/// @paramtable
class daWmCannon_c : public dWmObjActor_c {
public:
    daWmCannon_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmCannon_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void setCutEndSpecific(int cutsceneCommandId, bool param2);

    void setClipSphere(); ///< Sets the @ref mClipSphere "clip sphere" for the actor.
    void createModel(); ///< Initializes the resources for the actor.
    void calcModel(); ///< Updates the model's transformation matrix.

    dHeapAllocator_c mAllocator; ///< The allocator.
    m3d::smdl_c mModel; ///< The model.
    u32 mUnk200; ///< @unused
};
