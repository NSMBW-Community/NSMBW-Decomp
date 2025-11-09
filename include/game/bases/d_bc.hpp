#pragma once

#include <game/bases/d_rc.hpp>

/// @unofficial
class dBcSensor_c {
public:
    u32 mFlags; ///< &1 = is dBcSensor_extra_c
    int mOffsetX;
    int mOffsetY;
};

/// @unofficial
class dBcSensor_extra_c {
public:
    dBcSensor_c mBase;
    int m_0c;
};

/// @unofficial
class dBcSensor_extra_2_c {
public:
    dBcSensor_extra_c mBase;
    int m_10;
    int m_14;
};

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();

    void set(dActor_c *owner, const dBcSensor_c *foot, const dBcSensor_c *head, const dBcSensor_c *wall); ///< @unofficial

    void checkLink();
    bool checkRide();
    bool checkHead(unsigned long);
    int getSakaMoveAngle(u8);
    u32 checkWall(float *);
    u32 checkWallEnm(float *);

    u32 checkFootEnm();
    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }
    u32 getFootAttr();

    void checkBombBreak(mVec2_c, mVec2_c);

    enum WATER_TYPE_e {
        WATER_CHECK_NONE,
        WATER_CHECK_WATER,
        WATER_CHECK_WATER_BUBBLE,
        WATER_CHECK_YOGAN,
        WATER_CHECK_POISON
    };

    dActor_c *mpOwner;
    dBcSensor_c *mpSensorFoot;
    dBcSensor_c *mpSensorHead;
    dBcSensor_c *mpSensorWall;
    mVec3_c *mpOwnerPos;
    mVec3_c *mpOwnerLastPos;
    mVec3_c *mpOwnerSpeed;
    mVec3_c mPushForce;
    mVec2_c mOwnerPosDelta;
    mVec2_c m_34;
    mVec2_c m_3c;
    float m_44;
    float m_48;
    float m_4c;
    dRc_c *mpRc;
    char mPad2[0x34];
    u32 mFlags;
    char mPad3[0xc];
    s8 mOwningPlayerNo;
    char mPad4[0x4c];
    u8 mAmiLine;
    char mPad5[0x6];
    u8 mLayer;

    static bool checkWall(const mVec3_c *, const mVec3_c *, float *, unsigned char, unsigned char, dActor_c **);
    static int getUnitType(float, float, u8);
    static int checkWaterDepth(float, float, u8, u8, float *);
    static WATER_TYPE_e checkWater(float, float, u8, float *);
    static bool checkBg(float, float, u8, u8, unsigned long);

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
