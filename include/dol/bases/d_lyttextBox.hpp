#pragma once
#include <lib/nw4r/lyt/lyt_textBox.hpp>
#include <dol/bases/d_message.hpp>

class LytTextBox_c : public nw4r::lyt::TextBox {
public:
    void setMessage(MsgRes_c *bmg, unsigned long messageID, unsigned long messageGroup, long, ...);

    /// @unofficial
    static void SetTextInt(int *value, int *fillLeft, LytTextBox_c *textBox, bool fullWidth);
};
