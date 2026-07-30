#pragma once

#include <game/bases/d_actor_state.hpp>

class daBossDemo_c : public dActorState_c {
public:
    virtual ~daBossDemo_c();

    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, Ready);
    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, BattleStDemo);
    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, BattleIn);
    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, BattleEdDemo);

    virtual void beatLevel(); ///< @unofficial
    virtual void exitLevel(); ///< @unofficial
    virtual void startBGM();
    virtual void stopBGM();
    virtual u32 getBossID();
    virtual void setBossID();
    virtual void checkBattleStDemo();
    virtual void checkBattleEdDemo();
    virtual void demoScroll();
    virtual void bossSearch();
    virtual void initialize();

    int mCounter;
    u32 mIsBattleEndDemo;
    u32 mBossID;
};
