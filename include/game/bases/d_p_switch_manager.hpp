#pragma once

class dPSwManager_c {
public:
    enum SwType_e {
        SW_TYPE_0
    };

    bool checkSwitch(SwType_e);

    static dPSwManager_c *ms_instance;
};
