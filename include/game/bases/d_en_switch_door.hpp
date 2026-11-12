#pragma once

#include <game/bases/d_en_door.hpp>

/// @brief An event-activated d-pad door.
/// @ingroup bases
/// @unofficial
class dEnSwitchDoor_c : public dEnDoor_c {
public:
    dEnSwitchDoor_c() : mIsEventActivated(FALSE) {}

    virtual int preDraw() override;

    virtual void initialize() override;
    virtual void waitProc() override;

    BOOL mIsEventActivated;
};
