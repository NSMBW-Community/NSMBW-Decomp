#include <cstdarg>
#include <game/bases/d_lyttextBox.hpp>
#include <game/bases/d_font_manager.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_lytbase.hpp>

void LytTextBox_c::setMessage(MsgRes_c *bmg, ulong messageGroup, ulong messageID, long param, ...) {
    dInfo_c *info = dInfo_c::getInstance();
    info->mTextBoxMessageGroup = messageGroup;
    info->mTextBoxMessageID = messageID;

    va_list args;
    va_start(args, param);
    setMessage(bmg, messageGroup, messageID, param, &args);
    va_end(args);
}

void LytTextBox_c::ExtensionUserDataSetup() {
    dInfo_c *info = dInfo_c::getInstance();
    const nw4r::lyt::res::ExtUserData *userData = FindExtUserDataByName("shadow");
    if (userData != nullptr && *(int *) (((u8 *) userData) + userData->datOffs) != 0) {
        info->field_3da = true;
    }
}

void LytTextBox_c::setMessage(MsgRes_c *bmg, ulong messageGroup, ulong messageID, long param, va_list *vargs) {
    nw4r::lyt::Size fontSize = GetFontSize();

    u8 fontIndex = bmg->getFont(messageGroup, messageID);
    SetFont(dFontMng_c::getFont(fontIndex));
    LytBase_c::s_TagPrc.mFontIndex = fontIndex;

    dInfo_c *info = dInfo_c::getInstance();
    info->field_3da = 0;

    u16 extUserDataNum = GetExtUserDataNum();
    if (extUserDataNum != 0) {
        ExtensionUserDataSetup();
    }
    info->mTextBoxMessageGroup = messageGroup;
    info->mTextBoxMessageID = messageID;
    setText(bmg->getMsg(messageGroup, messageID), param, vargs, bmg);

    float charWScale = bmg->getScale(messageGroup, messageID) * 0.01f;
    fontSize.width = GetFont()->GetWidth() * charWScale;
    SetFontSize(fontSize);

    nw4r::lyt::Size size = GetSize();
    nw4r::lyt::Size newSize = size;
    newSize.width = 1024.0f;
    SetSize(newSize);

    nw4r::ut::WideTextWriter textWriter;

    float prevSpaceSize = GetCharSpace();
    nw4r::ut::Rect rectWithSpace = GetTextDrawRect(&textWriter);
    SetCharSpace(0.0f);
    TagProcessor_c::isZeroWidthSpace = true;
    nw4r::ut::Rect rectNoSpace = GetTextDrawRect(&textWriter);
    TagProcessor_c::isZeroWidthSpace = false;
    SetCharSpace(prevSpaceSize);
    SetSize(size);

    float widthNoSpace = rectNoSpace.GetWidth();
    float widthWithSpace = rectWithSpace.GetWidth();
    float widthDiff = widthWithSpace - widthNoSpace;
    if (size.width < widthWithSpace && widthNoSpace > 0.0f) {
        float prevScale = charWScale;
        charWScale *= (((size.width - widthDiff) - 4.0f) / widthNoSpace);
        if (charWScale < prevScale) {
            fontSize.width = GetFont()->GetWidth() * charWScale;
            SetFontSize(fontSize);
        }
    }
}

void LytTextBox_c::setText(const wchar_t *text, long param, ...) {
    va_list args;
    va_start(args, param);
    setText(text, param, &args, nullptr);
    va_end(args);
}

void LytTextBox_c::setText(const wchar_t *text, long param, va_list *vargs, MsgRes_c *bmg) {
    static wchar_t Buffer[0x200];
    int count = 0;
    LytBase_c::s_TagPrc.preProcess(text, Buffer, ARRAY_SIZE(Buffer), &count, param, vargs, bmg);
    SetString(Buffer, 0, count);
}
