#ifndef NW4R_UT_COLOR_H
#define NW4R_UT_COLOR_H
#include <nw4r/types_nw4r.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ut {

struct Color : public GXColor {
public:
    Color() {
        *this = WHITE;
    }
    Color(ulong color) {
        *this = color;
    }
    Color(int red, int green, int blue, int alpha) {
        Set(red, green, blue, alpha);
    }
    Color(const GXColor& rColor) {
        *this = rColor;
    }

    ~Color() {}

    void Set(int red, int green, int blue, int alpha) {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    Color& operator=(ulong color) {
        Toulongref() = color;
        return *this;
    }
    Color& operator=(const GXColor& rColor) {
        *this = *reinterpret_cast<const ulong*>(&rColor);
        return *this;
    }

    Color operator|(ulong color) const {
        return Color(Toulong() | color);
    }
    Color operator&(ulong color) const {
        return Color(Toulong() & color);
    }

    ulong& Toulongref() {
        return *reinterpret_cast<ulong*>(this);
    }
    const ulong& Toulongref() const {
        return *reinterpret_cast<const ulong*>(this);
    }

    ulong Toulong() const {
        return Toulongref();
    }

    operator ulong() const {
        return Toulongref();
    }

    // clang-format off
    static const ulong RED   = 0xFF0000FF;
    static const ulong GREEN = 0x00FF00FF;
    static const ulong BLUE  = 0x0000FFFF;

    static const ulong CYAN    = 0x00FFFFFF;
    static const ulong MAGENTA = 0xFF00FFFF;
    static const ulong YELLOW  = 0xFFFF00FF;

    static const ulong BLACK = 0x000000FF;
    static const ulong GRAY  = 0x808080FF;
    static const ulong WHITE = 0xFFFFFFFF;
    // clang-format on
} ALIGN(4);

} // namespace ut
} // namespace nw4r

#endif
