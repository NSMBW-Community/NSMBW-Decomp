#pragma once

#include <game/bases/d_en_castle_door.hpp>

/// @brief A blue door used in towers.
/// @ingroup bases
/// @unofficial
class dEnTorideDoor_c : public daEnDoor_c {
public:
    virtual ~dEnTorideDoor_c() {}
    virtual bool isClosed() override;
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
