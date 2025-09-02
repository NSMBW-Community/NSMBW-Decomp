#pragma once
#include <nw4r/lyt.h>
#include <game/bases/d_message.hpp>

class LytTextBox_c : public nw4r::lyt::TextBox {
public:
    void setMessage(MsgRes_c *bmg, ulong messageGroup, ulong messageID, long param, ...);
    void setMessage(MsgRes_c *bmg, ulong messageGroup, ulong messageID, long param, va_list *vargs);
    void ExtensionUserDataSetup();
    void setText(const wchar_t *text, long param, ...);
    void setText(const wchar_t *text, long param, va_list *vargs, MsgRes_c *bmg);
};
