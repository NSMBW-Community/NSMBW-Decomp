#pragma once

#include <game/bases/d_rc.hpp>

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();
    void checkLink();
    bool checkRide();

    char pad1[0x4c];
    dRc_c *rcRef;
    char pad2[0x34];
    u32 mFlags;
    char pad3[0x59];
    u8 _e5;
    char pad4[0x6];
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
