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

/// @unofficial
class dCdCourseSettings_c {
public:

    enum FLAG_e {
        WRAP_AROUND_EDGES = BIT_FLAG(0),
    };

    char pad[8];
    u16 mFlags;
};

class dCdFile_c {
public:
    char pad[0xc];
    dCdCourseSettings_c *mpCourseSettings;
    char pad2[0x1c];
    AreaBoundU16 *mpAreas;
    char pad3[0x380];

    u8 getAreaNo(mVec3_c *);
    AreaBoundU16 *getAreaP(u8 zoneID, mBoundBox *bound); ///< @unofficial
};

class dCd_c {
public:
    dCdFile_c mFiles[4];

    dCdFile_c *getFileP(int idx) {
        dCdFile_c *course = &mFiles[idx];
        if (course->mpAreas != nullptr) {
            return course;
        }
        return nullptr;
    }

    static dCd_c *m_instance;
};
