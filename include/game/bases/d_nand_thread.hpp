#pragma once

#include <lib/egg/core/eggHeap.h>

class dNandThread_c {
public:
    void cmdExistCheck();
    void cmdSpaceCheck();

    static void create(EGG::Heap *heap);

    u8 mPad[0x74];
    int mState;
    int mError;
    bool mFileExists;

    static dNandThread_c *m_instance;
};
