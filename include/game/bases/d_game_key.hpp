#pragma once
#include <types.h>

class Remocon {
public:
    /// @unofficial
    enum EXTENSION_TYPE_e {
        EXTENSION_NONE = 0, ///< No Wii remote extension is connected.
        EXTENSION_NUNCHUK, ///< The nunchuck controller is connected.
        EXTENSION_UNKNOWN = 3, ///< It has not been checked yet whether an extension is connected.
    };

    virtual ~Remocon();

    int mIndex;
    EXTENSION_TYPE_e mAttachedExtension;
    char mPad1[8];
    int mDownButtons; ///< The buttons currently pressed down.
    char mPad2[4];
    int mTriggeredButtons; ///< The buttons newly pressed down.
    char mPad3[0x6c];
    bool mIsShaking;
};

class dGameKeyCore_c : public Remocon {
public:
    u32 getAccVerticalAngleX();
};

class dGameKey_c {
public:
    virtual ~dGameKey_c();

    dGameKeyCore_c *mRemocon[4];

    short getAccVerticalAngleX(int plNo) {
        return mRemocon[plNo]->getAccVerticalAngleX() / 65536;
    }

    static dGameKey_c *m_instance;
};
