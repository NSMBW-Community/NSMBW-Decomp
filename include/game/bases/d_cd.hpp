#pragma once

class dCdArea_c {
public:
    AreaBoundU16 bound;
};

class dCdUnk_c {
public:
    char pad[8];
    u16 unk;
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
    short m_02;
    int m_04;
    int m_08;
    int m_0c;
};

class dCdFile_c {
public:
    char pad[0xc];
    dCdUnk_c *mpUnk;
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
    dCdArea_c *getAreaP(u8 zoneID, AreaBound *bound);
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
