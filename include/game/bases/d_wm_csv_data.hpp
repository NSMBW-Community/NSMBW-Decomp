#pragma once

class dCsvData_c {
public:
    const char *GetPointName(int) const;

    u8 mPad[0x16518];

public:
    static int c_COURSE_ID;
    static int c_GHOST_ID;
    static int c_TOWER_ID;
    static int c_CASTLE_ID;
    static int c_KINOKO_ID;
    static int c_ENEMY_ID;
    static int c_CANON_ID;
    static int c_TRSHIP_ID;
    static int c_AIRSHIP_ID;
    static int c_START_ID;
    static int c_PEACH_ID;
};
