#pragma once
#include <types.h>
#include <lib/nw4r/math/vec.hpp>
#include <lib/nw4r/math/mtx.hpp>
#include <lib/nw4r/ut/rect.hpp>

namespace nw4r {
namespace lyt {

class DrawInfo {
public:
    DrawInfo();
    virtual ~DrawInfo();

private:
    math::MTX34 mMtx;
    ut::Rect mViewRect;
    math::VEC2 mRelativeScale;
    float mAlpha;
    u8 mFlags;
};

} // namespace lyt
} // namespace nw4r
