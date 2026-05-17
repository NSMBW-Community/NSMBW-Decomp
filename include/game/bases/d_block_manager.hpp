#pragma once

#include <game/mLib/m_vec.hpp>

class dBlockMng_c {
public:
    struct BlockInfo_t {
        int mX, mY;
        u8 m_08;
        s8 m_09;
    };

    struct BlockInfo2_t {
        int mX, mY;
        int m_08;
        int m_0c;
        u8 m_10;
        u8 m_11;
        u8 m_12;
    };

    struct BlockInfo3_t {
        int mX, mY;
        int mKind;
        u8 m_0c;
        u8 m_0d;
        u8 m_0e;
        u8 m_0f;
    };

    void fn_80088b30(const BlockInfo_t &info);
    void fn_80088970(const BlockInfo2_t &info);
    bool fn_800883e0(const BlockInfo3_t &info);

    void chikuwaCreate_set(mVec2_c *);

    static dBlockMng_c *m_instance;
};
