#pragma once
#include <types.h>
#include <nw4r/ut.h>

namespace m2d {

/// @brief The draw order for 2D layouts.
enum LAYOUT_DRAW_ORDER_e {
    DRAW_ORDER_COURSE_SELECT_GUIDE = 2,
    DRAW_ORDER_SMALL_SCORE = 7,
    DRAW_ORDER_DEFAULT = 128, ///< The default draw order.
    DRAW_ORDER_PAUSE_WINDOW = 141,
    DRAW_ORDER_YES_NO_WINDOW = 143,
    DRAW_ORDER_WIPE = 154,
};

class Base_c : nw4r::ut::Link {
public:
    Base_c() : mDrawOrder(DRAW_ORDER_DEFAULT) {}
    virtual ~Base_c() {}
    virtual void draw();

    void entry();

    u8 mDrawOrder;
};

} // namespace m2d
