#pragma once

#include <game/mLib/m_vec.hpp>

class dActorMng_c {
public:
    void FUN_80066630(const mVec3_c &, u8, int, u8); ///< @unofficial

    static dActorMng_c *m_instance;
};
