#pragma once

#include <game/framework/f_profile.hpp>
#include <lib/egg/core/eggExpHeap.h>

class DynamicModuleControlBase {
public:
    bool load_async();
    bool link();
};

class dDynamicModuleControl : public DynamicModuleControlBase {
public:
    dDynamicModuleControl(const char *name, EGG::ExpHeap *heap);
    virtual ~dDynamicModuleControl();

    u8 mPad[0x44];
};

namespace dDyl {
    int LinkASync(fProfile::PROFILE_NAME_e);
    bool Unlink(fProfile::PROFILE_NAME_e);
};
