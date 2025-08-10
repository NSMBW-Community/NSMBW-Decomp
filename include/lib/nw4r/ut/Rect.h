#pragma once

namespace nw4r {
namespace ut {

struct Rect {
    Rect() {}
    Rect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}

    float GetWidth() const { return right - left; }

    float GetHeight() const { return bottom - top; }

    float left;
    float top;
    float right;
    float bottom;
};

} // namespace ut
} // namespace nw4r
