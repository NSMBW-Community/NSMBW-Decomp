#pragma once

#include <game/mLib/m_vec.hpp>

class dWmSeManager_c {
public:
    /// @unofficial
    int playSound(int id, int, const mVec3_c &pos, int, int);
    int playSound(int id, const mVec3_c &pos, int);
    void endSound(int id);

    static dWmSeManager_c *m_pInstance;
};
