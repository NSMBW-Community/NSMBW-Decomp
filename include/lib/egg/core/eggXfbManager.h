#pragma once

#include <lib/egg/core/eggXfb.h>

namespace EGG {

class XfbManager {
public:
    XfbManager() :
        m_00(nullptr),
        m_04(nullptr),
        m_08(nullptr),
        m_0c(0),
        m_0d(0) {}

    void attach(Xfb *xfb);

private:
    Xfb *m_00;
    Xfb *m_04;
    Xfb *m_08;
    u8 m_0c;
    u8 m_0d;
};

} // namespace EGG
