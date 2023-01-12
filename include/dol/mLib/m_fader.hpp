#pragma once
#include <dol/mLib/m_fader_base.hpp>

class mFader_c {
public:
    static bool isStatus(mFaderBase_c::EStatus status) { return mFader->getStatus() == status; }
    static mFaderBase_c *mFader;
};
