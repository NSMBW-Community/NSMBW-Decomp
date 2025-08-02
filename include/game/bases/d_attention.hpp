#pragma once
#include <game/framework/f_base.hpp>
#include <game/mLib/m_vec.hpp>

class dAttention_c {
public:
    void entry(fBaseID_e id);
    fBase_c *search(mVec3_c pos);
    fBase_c *searchPlayer(const dActor_c *player, mVec3_c pos);

    static dAttention_c *mspInstance;
};
