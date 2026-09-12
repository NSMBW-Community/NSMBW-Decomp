#pragma once

#include <types.h>

namespace EGG {

/// @unofficial
struct MsgEntry {
    u8 mPad[4];
    u16 mScale;
    u8 mFont;
};

class MsgRes {
private:
    u8 mPad[0x1c];

public:
    MsgRes(const void *p);
    virtual ~MsgRes();

    wchar_t *getMsg(ulong messageGroup, ulong messageID);
    MsgEntry *getMsgEntry(ulong messageGroup, ulong messageID);

};

} // namespace EGG
