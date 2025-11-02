#pragma once

#include <game/bases/d_cd_data.hpp>

class dActorGroupIdMng_c {
public:
    void process(sActorCreateData *data, int groupID, u8 file, int idx);

    static dActorGroupIdMng_c *m_instance;
};
