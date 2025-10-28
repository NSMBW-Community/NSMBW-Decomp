#pragma once
#include <nw4r/lyt.h>
#include <game/bases/d_message.hpp>

/// @brief A text box class with BMG message support.
class LytTextBox_c : public nw4r::lyt::TextBox {
public:
    /// @brief Sets the message to display in the text box.
    /// @param bmg The BMG resource.
    /// @param messageID The ID of the message to display.
    /// @param messageGroup The group of the message to display.
    /// @param placeholderCount The number of placeholders in the message.
    /// @param ... Values for placeholders in the message.
    void setMessage(MsgRes_c *bmg, ulong messageGroup, ulong messageID, long placeholderCount, ...);

    /// @brief Sets the text to display in the text box.
    /// @param text The text to display.
    /// @param placeholderCount The number of placeholders in the message.
    /// @param ... Values for placeholders in the text.
    void setText(const wchar_t *text, long placeholderCount, ...);

    void ExtensionUserDataSetup(); ///< Checks for extension user data in the layout.

private:
    /// @brief Sets the message to display in the text box with a va_list.
    /// @param bmg The BMG resource.
    /// @param messageID The ID of the message to display.
    /// @param messageGroup The group of the message to display.
    /// @param placeholderCount The number of placeholders in the message.
    /// @param vargs A va_list of values for placeholders in the message.
    void setMessage(MsgRes_c *bmg, ulong messageGroup, ulong messageID, long placeholderCount, va_list *vargs);

    /// @brief Sets the text to display in the text box with a va_list.
    /// @param text The text to display.
    /// @param placeholderCount The number of placeholders in the text.
    /// @param vargs A va_list of values for placeholders in the text.
    /// @param bmg The BMG resource.
    void setText(const wchar_t *text, long placeholderCount, va_list *vargs, MsgRes_c *bmg);
};
