#pragma once

#include <game/bases/d_wm_demo_actor.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_wm_map_model.hpp>

class daWmMap_c : public dWmDemoActor_c {
public:
    int FUN_80100830(int); ///< @unofficial

    dHeapAllocator_c mAllocator;
    dWmMapModel_c mModels[4];
    u8 mPad1[0x20c];
    int currIdx;
    u8 mPad2[0x4];
    dCsvData_c mCsvData[4];
    dWmConnect_c mWmConnect[4];

    dCsvData_c *GetCsvData() { return &mCsvData[currIdx]; }
    dWmConnect_c *GetWmConnect() { return &mWmConnect[currIdx]; }

    static daWmMap_c *m_instance;
};
