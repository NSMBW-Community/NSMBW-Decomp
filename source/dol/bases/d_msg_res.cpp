#include <game/bases/d_message.hpp>

MsgRes_c::MsgRes_c(const void *p, EGG::Heap *heap) : EGG::MsgRes(p) {}

MsgRes_c::~MsgRes_c() {}

u16 MsgRes_c::getScale(ulong messageGroup, ulong messageID) {
    return getMsgEntry(messageGroup, messageID)->mScale;
}

u8 MsgRes_c::getFont(ulong messageGroup, ulong messageID) {
    return getMsgEntry(messageGroup, messageID)->mFont;
}
