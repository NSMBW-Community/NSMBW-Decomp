#pragma once

#include <game/bases/d_a_en_door.hpp>

/// @unofficial
class dEnCastleDoor_c : public daEnDoor_c {
public:
    dEnCastleDoor_c() {}

    virtual void createMdl() override;
    virtual void initAnm() override;
    virtual void initCcData() override;
    virtual void initialize() override;
    virtual void setOpenAnm() override;
    virtual void setOpenSE() override;
    virtual void setCloseAnm() override;
    virtual void setCloseSE() override;
    virtual void setCloseMoveSE() override;
    virtual void setWaitAnm() override;
};
