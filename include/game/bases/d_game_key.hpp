#pragma once
#include <types.h>

class dGameKeyCore_c {
public:
    enum EXTENSION_e {
        EXTENSION_NONE = 0,
        EXTENSION_NUNCHUK = 1
    };

    virtual ~dGameKeyCore_c() {}

    int mIndex;
    EXTENSION_e mAttachedExtension;
    char mPad1[8];
    int mDownButtons; ///< The buttons currently pressed down.
    char mPad2[4];
    int mTriggeredButtons; ///< The buttons newly pressed down.
    char mPad3[0x6c];
    bool mIsShaking;
};

class dGameKey_c {
public:
    virtual ~dGameKey_c();

    dGameKeyCore_c *keys[4];

    static dGameKey_c *m_instance;
};
