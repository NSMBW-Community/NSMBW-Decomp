#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

/// @unofficial
struct AreaBoundU16 {
    AreaBoundU16(u16 x, u16 y, u16 w, u16 h) {
        this->x = x;
        this->y = y;
        width = w;
        height = h;
    }

    u16 x, y, width, height;
};

class dCdArea_c {
public:
    AreaBoundU16 bound;
};

/// @unofficial
class dCdCourseSettings_c {
public:

    enum FLAG_e {
        WRAP_AROUND_EDGES = BIT_FLAG(0),
    };

    char pad[8];
    u16 mFlags;
};

/// @unofficial
struct dScrollData_c {
    int mXMaybe;
    int mYMaybe;
    int mXCloudMaybe;
    int mYCloudMaybe;
    u16 m_12, mUpLimit;
    u16 m_16;
    u16 m_18;
};

struct dBgThing_c {
    mVec2_c makeVec() {
        return mVec2_c(m_00, -m_02);
    }

    short m_00;
    u16 m_02;
    int m_04;
    int m_08;
    int m_0c;
};

class dCdFile_c {
public:
    char pad[0xc];
    dCdCourseSettings_c *mpCourseSettings;
    char pad2[0x1c];
    dCdArea_c *areas;
    dBgThing_c (*mBgThings)[10];
    char pad3[0x70];
    int mScrlAreaDataSize;
    char pad4[0x308];

    u8 getAreaID(u8);
    u8 getAreaScroll(u8);
    u8 getAreaNo(mVec3_c *);
    dScrollData_c *getScrlAreaDataP(u8 scrollID);
    dScrollData_c *getScrollDataP(u8 scrollID);
    dCdArea_c *getAreaP(u8 zoneID, mBoundBox *bound);
};

class dCd_c {
public:
    dCdFile_c courses[4];

    dCdFile_c *getFileP(int i) {
        dCdFile_c *course = &courses[i];
        if (course->areas != nullptr) {
            return course;
        }
        return nullptr;
    }

    static dCd_c *m_instance;
};
