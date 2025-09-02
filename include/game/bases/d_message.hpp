#pragma once

#include <types.h>
#include <egg/core/eggMsgRes.h>

class MsgRes_c : public EGG::MsgRes {
public:
    u8 getFont(ulong messageGroup, ulong messageID);
    u16 getScale(ulong messageGroup, ulong messageID);
};

class dMessage_c {
public:
    static MsgRes_c *getMesRes();
};
