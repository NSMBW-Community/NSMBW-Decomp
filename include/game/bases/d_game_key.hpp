#pragma once

class Remocon {
public:
    virtual ~Remocon();

    enum EXTENSION_TYPE_e {
        EXTENTION_NONE,
        EXTENTION_ONE,
        EXTENTION_TWO,
        EXTENTION_THREE,
    };

    u8 pad[4];
    EXTENSION_TYPE_e mAttachedExtension;
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
