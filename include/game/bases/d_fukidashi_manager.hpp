#pragma once

struct dfukidashiInfo_c {
    bool mVisible;
    char pad[0x23b];
};

class dfukidashiManager_c {
public:
    char pad[0x381];
    dfukidashiInfo_c mInfos[4];

    static dfukidashiManager_c *m_instance;
};
