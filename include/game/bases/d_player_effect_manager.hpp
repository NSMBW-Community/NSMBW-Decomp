#pragma once

#include <game/mLib/m_vec.hpp>

class dPyEffect_c {
public:
    enum Type_e {
        TYPE_WATER_OUT_NORMAL,
        TYPE_WATER_OUT_MINI,
        TYPE_WATER_IN_NORMAL,
        TYPE_WATER_IN_MINI,
        TYPE_SMALL_SPLASH
    };
};

class dPyEffectMng_c {
public:
    void setEffect(dPyEffect_c::Type_e type, mVec3_c &, float, u8 layer);

    static dPyEffectMng_c *mspInstance;
};
