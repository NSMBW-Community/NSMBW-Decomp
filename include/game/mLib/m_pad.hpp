#pragma once

#include <lib/egg/core/eggController.h>

namespace mPad {
    void create();
    void beginPad();
    void endPad();

    extern EGG::CoreController *g_currentCore;
};
