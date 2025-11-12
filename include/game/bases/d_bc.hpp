#pragma once

#include <game/bases/d_rc.hpp>

/// @unofficial
class dBcSensor_c;

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();
    void checkLink();
    bool checkRide();
    bool checkHead(unsigned long);
    int getSakaMoveAngle(u8);
    void checkWall(float *);
    u32 checkWallEnm(float *);

    bool checkFootEnm();
    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }
    u32 getFootAttr();

    enum WATER_TYPE_e {
        WATER_CHECK_NONE,
        WATER_CHECK_WATER,
        WATER_CHECK_AIR_WATER,
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
    char mPad3[0x59];
    u8 mAmiLine;
    char mPad4[0x6];
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);
    static WATER_TYPE_e checkWater(float, float, u8, float *);
    static bool checkBg(float, float, u8, u8, unsigned long);

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
