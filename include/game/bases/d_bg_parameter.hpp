#pragma once

#include <types.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_actor.hpp>

class dBgParameter_c {
public:
    u16 getOneGetBuff(u16, u16, int);
    u16 setOneGetBuff(u16, u16, int, u16);

    u8 mPad1[0x30];
    mVec2_c m_30;
    mVec2_c mPos;
    mVec2_c mSize;
    mVec2_c mCenter;
    u8 mPad2[0x28];
    float m_78;

    const mVec2_c &pos() const { return mPos; }
    const mVec2_c &size() const { return mSize; }

    float posX() const { return mPos.x; }
    float posY() const { return mPos.y; }

    float width() const { return mSize.x; }
    float height() const { return mSize.y; }

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

    static float m_78_offset() { return 112.0f - getInstance()->m_78; }

    static dBgParameter_c *getInstance() { return ms_Instance_p; }

    static dBgParameter_c *ms_Instance_p;
};
