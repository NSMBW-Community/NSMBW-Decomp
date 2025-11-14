#pragma once

#include <game/mLib/m_vec.hpp>

class dActorMng_c {
public:
    void createUpCoin(const mVec3_c &pos, u8 dir, u8 count, u8 layer);

    static dActorMng_c *m_instance;
};
