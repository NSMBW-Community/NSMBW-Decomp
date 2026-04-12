#pragma once

#include <types.h>
#include <game/bases/d_actor_data.hpp>
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
    char mPad2[0x6];
    u8 m_10;
    char mPad3[0x2];
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
enum NextGotoType_e {
    NEXT_GOTO_TYPE_LEVEL_ENTRANCE,
    NEXT_GOTO_TYPE_1,
    NEXT_GOTO_TYPE_DOOR_EXIT,
    NEXT_GOTO_TYPE_PIPE_U,
    NEXT_GOTO_TYPE_PIPE_D,
    NEXT_GOTO_TYPE_PIPE_L,
    NEXT_GOTO_TYPE_PIPE_R,
    NEXT_GOTO_TYPE_FALL,
    NEXT_GOTO_TYPE_GROUND_POUND,
    NEXT_GOTO_TYPE_SLIDE,
    NEXT_GOTO_TYPE_SWIM,
    NEXT_GOTO_TYPE_11,
    NEXT_GOTO_TYPE_12,
    NEXT_GOTO_TYPE_13,
    NEXT_GOTO_TYPE_14,
    NEXT_GOTO_TYPE_15,
    NEXT_GOTO_TYPE_MINI_PIPE_U,
    NEXT_GOTO_TYPE_MINI_PIPE_D,
    NEXT_GOTO_TYPE_MINI_PIPE_L,
    NEXT_GOTO_TYPE_MINI_PIPE_R,
    NEXT_GOTO_TYPE_JUMP_R,
    NEXT_GOTO_TYPE_CLIMB,
    NEXT_GOTO_TYPE_WATER_TANK,
    NEXT_GOTO_TYPE_BOSS,
    NEXT_GOTO_TYPE_JUMP_L,
    NEXT_GOTO_TYPE_BOSS_JUMP_R,
    NEXT_GOTO_TYPE_BOSS_FALL,
    NEXT_GOTO_TYPE_DOOR_ENTER
};

/// @unofficial
enum NextGotoFlags_e {
    NEXT_GOTO_FLAG_RAIL_REVERSE = BIT_FLAG(0),
    NEXT_GOTO_FLAG_WATER_TANK = BIT_FLAG(2),
    NEXT_GOTO_FLAG_RAIL = BIT_FLAG(3),
    NEXT_GOTO_FLAG_ONLY_EXIT = BIT_FLAG(7)
};

/// @unofficial
// [Needs it like this (the relevant part is an inner struct) so that struct copies are done correctly]
union sNextGotoData {
    struct {
        u16 mX, mY;
        u8 mPad1[0x4];
        u8 mID;
        u8 mDestFile; ///< 0 = same file, others are 1-indexed.
        u8 mDestID;
        u8 mType; ///< See NextGotoType_e.
        u8 m_0c;
        u8 mArea;
        u8 mLayer;
        u8 mRailID;
        u16 mFlags; ///< See NextGotoFlags_e.
        bool mIsLevelExit;
        u8 m_13;
    };
    u8 mData[20];
};

/// @unofficial
struct sMapActorData {

    ProfileName getProfileName() {
        return dActorData_c::getMapActorInfo(mMapActorID)->mProfileName;
    }

    u16 mMapActorID;
    u16 mX, mY;
    u8 mEventNums[2];
    u32 mParam;
    u8 mAreaNo;
    u8 mLayer;
};

/// @unofficial
struct sBlock8Data {
    u8 mPad[4];
};

struct sRangePosSize {
    u16 mX, mY;
    u16 mWidth, mHeight;
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
    u8 mID;
    u16 mNodeIdx, mCount;
    u16 mFlags;
};

/// @unofficial
struct sRailNodeData {
    short mX, mY;
    float mSpeed;
    float mAccel;
    short mDelay;
    u16 mFlags;
};
