#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/smdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/// @brief The actor for non-moving pipes found on the World Map.
/// @details This actor's internal model is unused. The pipe model must be added to the World Map itself.
/// @ingroup bases
/// @paramtable
class daWmDokan_c : public dWmObjActor_c {
public:
    daWmDokan_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmDokan_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    dHeapAllocator_c mAllocator; ///< The allocator. @unused
    m3d::smdl_c mModel; ///< The model. @unused
};
