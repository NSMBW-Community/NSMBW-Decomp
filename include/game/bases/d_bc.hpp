#pragma once

#include <types.h>
#include <game/bases/d_rc.hpp>

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

    enum WaterCheckResult_e {
        R0 = 0,
        R1 = 1,
        R2 = 2,
        R3 = 3,
        R4 = 4
    };

    dActor_c *mpOwner;
    dBcSensor_c *mpSensorFoot;
    dBcSensor_c *mpSensorHead;
    dBcSensor_c *mpSensorWall;
    mVec3_c *mpOwnerPos;
    mVec3_c *mpOwnerLastPos;
    mVec3_c *mpOwnerSpeed;
    mVec3_c m_20;
    mVec2_c mOwnerPosDelta;
    mVec2_c m_34;
    mVec2_c m_3c;
    float m_44;
    float m_48;
    float m_4c;
    dRc_c *rcRef;
    char pad2[0x34];
    u32 mFlags;
    char pad3[0x59];
    u8 _e5;
    char pad4[0x6];
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);
    static WaterCheckResult_e checkWater(float, float, u8, float *);
    static void checkBg(float, float, u8, u8, unsigned long);

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot() NOINLINE {
        return mFlags & 0b00000000000111111110000000000000;
    }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
