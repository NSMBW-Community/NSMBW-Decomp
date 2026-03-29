#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/smdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

class daWmCannon_c : public dWmObjActor_c {
public:
    daWmCannon_c();
    ~daWmCannon_c();

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void setCutEndSpecific(int cutsceneId, bool param2);

    void setClipSphere();
    void loadModel();
    void calcModel();

    dHeapAllocator_c mAllocator;
    m3d::smdl_c mModel;
    u32 m_200;
};
