#pragma once
#include <types.h>

class dStageTimer_c {
public:
    virtual ~dStageTimer_c() {}

    int mTimeValue;

    short convertToIGT() const {
        return (mTimeValue + 4095) >> 12;
    }

    static dStageTimer_c *m_instance;
};
