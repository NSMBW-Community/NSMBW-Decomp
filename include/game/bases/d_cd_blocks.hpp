#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>

#define CD_BLOCK_COUNT 14

// @unofficial
struct sBlockInfo {
    u32 mOffset;
    u32 mSize;
};

/// @unofficial
struct sTilesetData {
    char mTileset0[32];
    char mTileset1[32];
    char mTileset2[32];
    char mTileset3[32];
};

/// @unofficial
class sOptionData {
public:
    enum FLAG_e {
        WRAP_AROUND_EDGES = BIT_FLAG(0),
    };

    char mPad1[0x8];
    u16 mFlags;
    char mPad2[0xa];
};

/// @unofficial
struct sScrollData {
    u8 mPad1[0x10];
    u16 mID;
    u8 mPad2[0x6];
};

/// @unofficial
struct sBlock4Data {
    u8 mPad[0x8];
};

/// @unofficial
struct sBgData {
    u16 mID;
    u8 mPad[0x16];
};

/// @unofficial
struct sNextGotoData {
    u8 mPad1[0x8];
    u8 mID;
    u8 mPad2[0xb];
};

/// @unofficial
struct sActorCreateData {
    int mPad1[3];
    u8 m_0c;
};

/// @unofficial
struct sBlock8Data {
    u8 mPad[4];
};

struct sRangePosSize {
    u16 mX, mY;
    u16 mWidth, mHeight;

    int getRight() const { return mX + mWidth / 2; }
    int getTop() const { return mY + mHeight / 2; }
};

struct sRangeData : sRangePosSize {
    u8 mID;
    u8 mPad[2];
};

/// @unofficial
struct sAreaData {
    sRangePosSize mRangeData;
    u16 mColorObj, mColorBG;
    u8 mID;
    u8 mScroll;
    u8 mZoomType, mZoomID, mZoomChange;
    u8 mMaskNo;
    u8 mBg2, mBg3;
    u8 mDirection;
    u8 m_15;
    u8 mAreaBgm, mBgmMode;
};

/// @unofficial
struct sScrollAreaData {
    u8 mPad[0x14];
};

/// @unofficial
struct sRailInfoData {
    u8 mPad[0x8];
};

/// @unofficial
struct sRailNodeData {
    u8 mPad[0x10];
};
