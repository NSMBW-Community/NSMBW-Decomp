#pragma once

#include <types.h>

class dNandThread_c {
public:
    void cmdExistCheck();
    void cmdSpaceCheck();

    u8 mPad[0x74];
    int mState;
    int mError;
    bool mFileExists;

    static dNandThread_c *m_instance;
};
