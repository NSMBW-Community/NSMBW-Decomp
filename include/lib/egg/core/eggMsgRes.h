#pragma once

#include <types.h>

namespace EGG {

class MsgRes {
public:
    wchar_t *getMsg(ulong messageGroup, ulong messageID);
};

} // namespace EGG
