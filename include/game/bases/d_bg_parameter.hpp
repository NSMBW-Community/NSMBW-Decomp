#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_actor.hpp>

class dBgParameter_c {
public:
    /// @unofficial
    enum BG_SCROLL_DIR_X_e {
        BG_SCROLL_X_NONE,
        BG_SCROLL_X_LEFT,
        BG_SCROLL_X_RIGHT,
    };

    /// @unofficial
    enum BG_SCROLL_DIR_Y_e {
        BG_SCROLL_Y_NONE,
        BG_SCROLL_Y_DOWN,
        BG_SCROLL_Y_UP,
    };

    u8 mPad1[0x38];
    mVec2_c mPos;
    mVec2_c mSize;
    u8 mPad2[0x38];
    u8 mScrollDirX; ///< See BG_SCROLL_DIR_X_e.
    u8 mScrollDirY; ///< See BG_SCROLL_DIR_Y_e.

    const mVec2_c &pos() const { return mPos; }
    const mVec2_c &size() const { return mSize; }

    float xStart() const { return pos().x; }
    float yStart() const { return pos().y; }
    float xSize() const { return size().x; }
    float ySize() const { return size().y; }
    float xEnd() const { return xStart() + xSize(); }
    float yEnd() const { return yStart() - ySize(); }

    void getBounds(float &startX, float &startY, float &endX, float &endY) const {
        startX = xStart();
        startY = yStart();
        endX = xEnd();
        endY = yEnd();
    }

    bool check(float y, float offset) {
        return y - offset > yStart();
    }

    static dBgParameter_c *getInstance() { return ms_Instance_p; }

    static dBgParameter_c *ms_Instance_p;
};
