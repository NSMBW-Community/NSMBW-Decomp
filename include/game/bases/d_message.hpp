#pragma once

#include <types.h>
#include <egg/core/eggMsgRes.h>

class MsgRes_c : public EGG::MsgRes {
public:
    u8 getFont(ulong messageID, ulong messageGroup);
    u16 getScale(ulong messageID, ulong messageGroup);
};

class dMessage_c {
public:
    static MsgRes_c *getMesRes();
};
