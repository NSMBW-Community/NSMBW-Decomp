#include <game/mLib/m_color.hpp>

void mColor::lerp(const GXColor &c1, const GXColor &c2, float t) {
    float s = 1.0f - t;
    r = c1.r * s + c2.r * t;
    g = c1.g * s + c2.g * t;
    b = c1.b * s + c2.b * t;
    a = c1.a * s + c2.a * t;
}
