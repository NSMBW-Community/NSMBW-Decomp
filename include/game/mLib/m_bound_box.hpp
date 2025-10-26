#pragma once

#include <game/mLib/m_vec.hpp>

/// @unofficial
class mBoundBox {
public:
    mBoundBox() {}

    mBoundBox(float t, float b, float l, float r) {
        set(t, b, l, r);
    }

    mBoundBox(const mBoundBox &b) {
        set(b.mOffset.x, b.mOffset.y, b.mSize.x, b.mSize.y);
    }

    void set(float t, float b, float l, float r) {
        mOffset.set(t, b);
        mSize.set(l, r);
    }

    mVec2_c getSize() const {
        return mSize;
    }

    mVec2_c mOffset, mSize;
};
