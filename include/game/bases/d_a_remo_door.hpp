#pragma once
#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d.hpp>
#include <lib/nw4r/g3d/res_file.hpp>

class daRemoDoor_c : public dActorState_c {
public:
    daRemoDoor_c() : mRes(nullptr) {}
    ~daRemoDoor_c() {}

    virtual int create();
    virtual int execute();
    virtual int draw();

    void init();

    STATE_FUNC_DECLARE(daRemoDoor_c, OpenReady);
    STATE_FUNC_DECLARE(daRemoDoor_c, Open);
    STATE_FUNC_DECLARE(daRemoDoor_c, Close);

    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mRes;
    m3d::mdl_c mDoorModel;

    static const int SOUND_CLOSE, SOUND_OPEN;
};
