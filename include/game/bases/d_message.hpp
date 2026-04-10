#pragma once

#include <types.h>
#include <egg/core/eggHeap.h>
#include <egg/core/eggMsgRes.h>

class MsgRes_c : public EGG::MsgRes {
public:
    u8 getFont(ulong messageID, ulong messageGroup);
    u16 getScale(ulong messageID, ulong messageGroup);
};

class dMessage_c {
public:
    static bool create(EGG::Heap *heap);
    static MsgRes_c *getMesRes();
};
