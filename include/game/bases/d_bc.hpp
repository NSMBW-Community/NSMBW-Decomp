#pragma once

#include <game/bases/d_rc.hpp>

class dBg_ctr_c {
public:
    dActor_c *mpActor;
    u8 mPad1[0xb8];
    short *m_bc;

    void addDokanMoveDiff(mVec3_c *);
};

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();

    bool checkRide();
    void checkLink();

    char mPad1[0x4c];
    dRc_c *mpRc;
    dActor_c *mpActor;
    dBg_ctr_c *mpCtrHead;
    dBg_ctr_c *mpCtrFoot;
    dBg_ctr_c *mpCtrWall;
    char mPad3[0x24];
    u32 mFlags;
    char mPad4[0xc];
    u8 mOwningPlrNo;
    char mPad5[0x23];
    u8 mPlayerFlags;
    char mPad6[0x1c];
    float mIceSpeed;
    char mPad7[0x5];
    u8 mLineKind;
    char mPad8[0x6];
    u8 mLayer;

    static int checkWater(float, float, u8, float *);
    static int checkWaterDepth(float, float, u8, u8, float *);
    static bool checkGround(const mVec3_c *, float *, u8, u8, s8);
    static bool checkTenjou(const mVec3_c &, float &, u8, u8);
    bool checkRoofPlayer(const mVec3_c *, float *);
    u16 getFootAttr();
    u32 getSakaDir();
    int checkDokanLR(mVec3_c *, u8, int *, float, float);
    int checkDokanDown(mVec3_c *, int *);
    int checkDokanUp(mVec3_c *, int *);
    void setRideOnObjBg(dBg_ctr_c *, const mVec3_c &);

    bool getSakaUpDown(u8 direction);
    short getSakaAngle(u8 direction);
    int getSakaType();

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
