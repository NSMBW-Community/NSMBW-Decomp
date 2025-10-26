#pragma once
#include <game/bases/d_base.hpp>

class dSelectCursor_c : public dBase_c {
public:
    void Cancel(int);

    u8 mPad[0xcc8];
    bool mHasLoadedLayout;

    static dSelectCursor_c *m_instance;
};
