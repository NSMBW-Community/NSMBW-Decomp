#pragma once
#include <game/bases/d_base.hpp>

enum UNIT_NUM_e {
    UNIT_0 = 0,
    UNIT_GRASS_L = 416, ///< Grass (left edge)
    UNIT_GRASS_C1, ///< Grass (center 1)
    UNIT_GRASS_C2, ///< Grass (center 2)
    UNIT_GRASS_C3, ///< Grass (center 3)
    UNIT_GRASS_R, ///< Grass (right edge)
    UNIT_FLOWER_1 = 432, ///< Yellow flower
    UNIT_FLOWER_2, ///< Red flower
    UNIT_FLOWER_3, ///< Blue flower
    UNIT_GRASS_FLOWER_1 = 448, ///< Grass with yellow flower
    UNIT_GRASS_FLOWER_2, ///< Grass with red flower
    UNIT_GRASS_FLOWER_3, ///< Grass with blue flower
};

class dBgUnit_c {
public:
    dBgUnit_c *GetBuffPos(u16, u16, int *, bool);

    static bool fn_80081900(u16, u16 *); ///< @unofficial
    static u16 cvtNum(u16); ///< @unofficial

    u16 mUnitNumber;
};

struct dBgData_c {
    u32 mLayer0;
    u32 mLayer1;
    u32 mLayer2;
};


class dBgGlobal_c {
public:
    virtual ~dBgGlobal_c();
    char *getEnvironment(int, int);
    dBgUnit_c *GetBgUnitP(int, int);

    dBgData_c mData[100];

    static dBgGlobal_c *ms_pInstance;
};
