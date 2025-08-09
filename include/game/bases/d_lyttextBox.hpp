#pragma once
#include <nw4r/lyt.h>
#include <game/bases/d_message.hpp>

class LytTextBox_c : public nw4r::lyt::TextBox {
public:
    void setMessage(MsgRes_c *bmg, ulong messageID, ulong messageGroup, long param, ...);
    void ExtensionUserDataSetup();
    void setMessage(MsgRes_c *bmg, ulong messageID, ulong messageGroup, long param, va_list *vargs);
    void setText(const wchar_t *text, long, ...);
    void setText(const wchar_t *text, long, va_list *vargs, MsgRes_c *bmg);
};
