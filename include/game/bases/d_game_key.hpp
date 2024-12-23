#pragma once

class Remocon {
public:
    virtual ~Remocon();

    enum EXTENSION_TYPE_e {
        EXTENSION_NONE = 0,
        EXTENSION_NUNCHUK = 1,
        EXTENSION_TWO,
        EXTENSION_THREE,
    };

    int mIndex;
    EXTENSION_TYPE_e mAttachedExtension;
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

    Remocon *mRemocon[4];

    static dGameKey_c *m_instance;
};
