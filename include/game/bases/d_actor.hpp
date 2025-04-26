#pragma once

#include <game/bases/d_base_actor.hpp>

class dActor_c : public dBaseActor_c {
public:
    static bool mExecStopReq;
    static bool mDrawStopReq;
    static bool mExecStop;
    static bool mDrawStop;
};
