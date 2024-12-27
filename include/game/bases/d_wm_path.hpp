#pragma once
#include <game/mLib/m_vec.hpp>

class dWmPath_c {
public:
    dWmPath_c();
    virtual ~dWmPath_c();

    mVec3_c points[32];
    int _, a, b, c;
};
