#pragma once

#include <game/bases/d_en_door.hpp>

/// @unofficial
class dEnSwitchDoor_c : public dEnDoor_c {
public:
    dEnSwitchDoor_c() : mIsEventActivated(0) {}

    virtual int preDraw() override;

    virtual void initialize() override;
    virtual void waitProc() override;

    BOOL mIsEventActivated;
};
