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
