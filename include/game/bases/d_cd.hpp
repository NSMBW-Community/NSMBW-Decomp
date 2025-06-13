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
class dCdUnk_c {
public:
    char pad[8];
    u16 mFlags;
};

class dNextGoto_c {
public:
    u8 mPad1[0xb];
    u8 m_0b;
    u8 mPad2[2];
    u8 mLayer;
    u8 m_0f;
    u16 mFlags;
};

/// @unofficial
struct dRailNode_s {
    s16 x;
    s16 y;
    float mSpeed;
    float mAccel;
    u16 mDelay;
    u16 m_e;
};

class dCdFile_c {
public:
    char pad[0xc];
    dCdUnk_c *mpUnk;
    char pad2[0x1c];
    AreaBoundU16 *mpAreas;
    u8 mPad3[0xc];
    dRailNode_s *mpRailNodes;
    u8 pad3[0x370];

    u8 getAreaNo(mVec3_c *);
    AreaBoundU16 *getAreaP(u8 zoneID, mBoundBox *bound); ///< @unofficial
    dNextGoto_c *getNextGotoP(u8);
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
