#pragma once

#include <game/sLib/s_Phase.hpp>
#include <nw4r/ut.h>

namespace dSys_c {
    void create();
    void setBlack(bool);
    void setFrameRate(u8);
    void setClearColor(nw4r::ut::Color color);
    void execute();
}

namespace dSystem {
    sPhase_c::METHOD_RESULT_e createFontManagerPhase(void *);
    sPhase_c::METHOD_RESULT_e createMessageManagerPhase(void *);
    sPhase_c::METHOD_RESULT_e createEffectManagerPhase1(void *);
    sPhase_c::METHOD_RESULT_e createEffectManagerPhase2(void *);
    void fixArena();
}
