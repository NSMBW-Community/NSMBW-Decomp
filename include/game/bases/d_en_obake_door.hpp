#pragma once

#include <game/bases/d_en_door.hpp>

/// @brief A ghost house door.
/// @ingroup bases
/// @unofficial
class dEnObakeDoor_c : public daEnDoor_c {
public:
    virtual bool isClosed() override;
    virtual bool isDummyOpen() override;
    virtual void createMdl() override;
    virtual void initAnm() override;
    virtual void initCcData() override;
    virtual void waitProc() override;
    virtual void setOpenAnm() override;
    virtual void setOpenSE() override;
    virtual void setCloseAnm() override;
    virtual void setCloseSE() override;
    virtual void setWaitAnm() override;
};
