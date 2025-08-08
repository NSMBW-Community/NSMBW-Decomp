#pragma once
#include <nw4r/lyt.h>
#include <game/bases/d_message.hpp>

class LytTextBox_c : public nw4r::lyt::TextBox {
public:
    void setMessage(MsgRes_c *bmg, unsigned long messageGroup, unsigned long messageID, long, ...);
};
