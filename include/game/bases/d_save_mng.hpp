#pragma once
#include <game/bases/d_mj2d_data.hpp>

class dSaveMng_c {
public:
    dMj2dGame_c *getSaveGame(s8);
    dMj2dGame_c *getTempGame(s8);
    bool isNandBusy();
    void calcCRC();
    void startNandSave();
    void startNandLoad();

    u8 mPad[0x20];
    dMj2dHeader_c mHeader;

    static dSaveMng_c *m_instance;
};
