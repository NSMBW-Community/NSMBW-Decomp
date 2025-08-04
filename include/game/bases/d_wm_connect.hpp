#pragma once
#include <types.h>

class dWmConnect_c {
public:
    /// @unofficial
    struct Point_s {
        u32 m_00;
        char name[32];
    };

    Point_s *GetPointFromIndex(int);
};
