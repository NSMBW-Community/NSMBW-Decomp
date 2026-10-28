#pragma once
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_yoshi_mdl.hpp>

class daYoshi_c : public daPlBase_c {
public:
    enum RideType_e {
        RIDE_TYPE_0
    };

    u8 mPad1[0x4c];
    int m_94;
    u8 mPad2[0x8];
    int m_a0;
    u8 mPad3[0x30];
    int mFruitCount;

    dYoshiMdl_c *getModel() { return (dYoshiMdl_c *) daPlBase_c::getModel(); }

    int getFruitCount() const { return mFruitCount; }

    bool checkRideOffAble();
    void setRideOffPlayer();
    bool getTongueTipMtx(mMtx_c *mtx);
    bool setFocedEatOutAction(dActor_c *player);
    bool setRideOnPlayer(daPlBase_c *player, RideType_e);
    void getMouthMtx(mMtx_c *mtx);
    daPlBase_c *getPlayerRideOn() const;
};
