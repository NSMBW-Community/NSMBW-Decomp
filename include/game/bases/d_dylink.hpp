#pragma once

#include <game/framework/f_profile.hpp>
#include <lib/egg/core/eggExpHeap.h>

class dDynamicModuleControl {
public:
    dDynamicModuleControl(const char *name, EGG::ExpHeap *heap);
    virtual ~dDynamicModuleControl();

    u8 mPad[0x44];
};

namespace dDyl {
    bool Unlink(fProfile::PROFILE_NAME_e);
};
