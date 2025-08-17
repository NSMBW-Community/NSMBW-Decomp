#pragma once
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

class dWmPath_c {
public:
    dWmPath_c();
    virtual ~dWmPath_c();

    mVec3_c mPoints[WM_PATH_MAX_POINT_COUNT];
    int _, a, b, c;
};
