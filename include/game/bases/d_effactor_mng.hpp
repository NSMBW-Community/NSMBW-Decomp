#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

class dEffActorMng_c {
public:
    void createWaterSplashEff(mVec3_c &, unsigned long, s8, mVec3_c);

    static dEffActorMng_c *m_instance;
};
