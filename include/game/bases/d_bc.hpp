#pragma once

#include <game/bases/d_rc.hpp>

class dBcSensor_c {
public:
    u32 mFlags;
    int x;
    int y;

// public:
//     dBcSensor_c(u32 f, int x, int y) : mFlags(f), x(x), y(y) {}
};

// class dBcPointSensor_c : public dBcSensor_c {
//     int x;
//     int y;

// public:
//     dBcPointSensor_c(u32 f, int x, int y) : dBcSensor_c(f), x(x), y(y) {}
// };

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();

    bool checkRide();
    void checkLink();

    char mPad1[0x4c];
    dRc_c *mpRc;
    char mPad2[0x34];
    u32 mFlags;
    char mPad3[0x59];
    u8 mLineKind;
    char mPad4[0x6];
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);
    static WaterCheckResult_e checkWater(float, float, u8, float *);
    static void checkBg(float, float, u8, u8, unsigned long);
    static bool checkWireNet(float x, float y, unsigned char layer);
    static bool checkGround(const mVec3_c *, float *, u8, u8, signed char);
    static bool findGround(mVec3_c *, float *, u32, u32); ///< @unofficial
    static u32 findWall(mVec3_c *, mVec3_c *, float *, u8, u32, dActor_c *); ///< @unofficial
    /// @unofficial
    void set(dActor_c *, const dBcSensor_c *, const dBcSensor_c *, const dBcSensor_c *);

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
