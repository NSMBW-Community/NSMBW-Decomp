#include <game/bases/d_a_player_base.hpp>

daPlBase_c::daPlBase_c() :
    mStateMgr(*this, sStateID::null),
    mDemoStateMgr(*this, sStateID::null)
{
    changeDemoState(StateID_DemoNone, 0);
    changeState(StateID_None);
}
