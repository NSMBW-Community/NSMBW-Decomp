#pragma once
#include <types.h>
#include <nw4r/ut.h>

namespace m2d {
namespace LAYOUT_DRAW_ORDER {

/// @brief The draw order for 2D layouts.
enum LAYOUT_DRAW_ORDER_e {
    COURSE_SELECT_GUIDE = 2,
    SMALL_SCORE = 7,
    DEFAULT = 128, ///< The default draw order.
    PAUSE_WINDOW = 141,
    YES_NO_WINDOW = 143,
    WIPE = 154,
};
}

class Base_c : nw4r::ut::Link {
public:
    Base_c() : mDrawOrder(m2d::LAYOUT_DRAW_ORDER::DEFAULT) {}
    virtual ~Base_c() {}
    virtual void draw();

    void entry();

    u8 mDrawOrder;
};

} // namespace m2d
