#pragma once
#include <types.h>

/// @unofficial
struct dRailInfo_s {
    enum FLAG_e {
        FLAG_0001 = BIT_FLAG(0),
        IS_LOOPED = BIT_FLAG(1),
        FLAG_0004 = BIT_FLAG(2),
        FLAG_0008 = BIT_FLAG(3),
        FLAG_0010 = BIT_FLAG(4),
        FLAG_0020 = BIT_FLAG(5),
        FLAG_0040 = BIT_FLAG(6),
        FLAG_0080 = BIT_FLAG(7),
        FLAG_0100 = BIT_FLAG(8),
        FLAG_0200 = BIT_FLAG(9),
        FLAG_0400 = BIT_FLAG(10),
        FLAG_0800 = BIT_FLAG(11),
        FLAG_1000 = BIT_FLAG(12),
        FLAG_2000 = BIT_FLAG(13),
        FLAG_4000 = BIT_FLAG(14),
        FLAG_8000 = BIT_FLAG(15)
    };

    u8 mId;
    u8 mPad[1];
    u16 mStartNodeIdx;
    u16 mNumNodes;
    u16 mFlags;
};

class dRail_c {
public:
    static dRailInfo_s *getRailInfoP(u8);
};
