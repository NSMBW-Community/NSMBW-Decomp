#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

class dWmConnect_c {
public:
    /// @unofficial
    struct Point_s {
        u32 m_00;
        char name[8];
        mVec3_c pos;
    };

    Point_s *GetPointFromIndex(int);
};
