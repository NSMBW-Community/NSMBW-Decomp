#pragma once

#include <game/bases/d_en_castle_door.hpp>

/// @brief A regular d-pad door actor.
/// @ingroup bases
/// @unofficial
class dEnDoor_c : public daEnDoor_c {
public:
    virtual bool isClosed() override;
    virtual void createMdl() override;
    virtual void initAnm() override;
    virtual void initCcData() override;
    virtual void setOpenAnm() override;
    virtual void setOpenSE() override;
    virtual void setCloseAnm() override;
    virtual void setCloseSE() override;
    virtual void setWaitAnm() override;
};
