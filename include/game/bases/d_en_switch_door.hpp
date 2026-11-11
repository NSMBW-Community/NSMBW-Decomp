#pragma once

#include <game/bases/d_en_door.hpp>

/// @unofficial
class dEnSwitchDoor_c : public dEnDoor_c {
public:
    dEnSwitchDoor_c() : m_5dc(0) {}

    virtual int preDraw() override;

    virtual void initialize() override;
    virtual void waitProc() override;

    u32 m_5dc;
};
