#pragma once

class dCsvData_c {
public:
    const char *GetPointName(int) const;

    u8 mPad[0x16518];

public:
    static const int c_COURSE_ID;
    static const int c_GHOST_ID;
    static const int c_TOWER_ID;
    static const int c_CASTLE_ID;
    static const int c_KINOKO_ID;
    static const int c_ENEMY_ID;
    static const int c_CANON_ID;
    static const int c_TRSHIP_ID;
    static const int c_AIRSHIP_ID;
    static const int c_START_ID;
    static const int c_PEACH_ID;
};
