#pragma once
#include <game/bases/d_base.hpp>

class dSelectCursor_c : public dBase_c {
public:
    void Cancel(int);

    static dSelectCursor_c *m_instance;
};
