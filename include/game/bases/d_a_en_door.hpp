#pragma once

#include <game/bases/d_enemy.hpp>

class daEnDoor_c : public dEn_c {
public:
    virtual bool isClosed();
    virtual bool isDummyOpen();
    virtual void createMdl();
    virtual void initAnm();
    virtual void initCcData();
    virtual void initialize();
    virtual void waitProc();
    virtual void setOpenAnm();
    virtual void setOpenSE();
    virtual void setCloseAnm();
    virtual void setCloseSE();
    virtual void setCloseMoveSE();
    virtual void setWaitAnm();

    u8 mPad[0xa8];
    int m_5cc;
    int m_5d0;
    int m_5d4;
};
