#include <cstdarg>
#include <game/bases/d_lyttextBox.hpp>
#include <game/bases/d_font_manager.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_lytbase.hpp>

void LytTextBox_c::setMessage(MsgRes_c *bmg, ulong messageID, ulong messageGroup, long placeholderCount, ...) {
    dInfo_c *info = dInfo_c::getInstance();
    info->mTextBoxMessageID = messageID;
    info->mTextBoxMessageGroup = messageGroup;

    va_list args;
    va_start(args, placeholderCount);
    setMessage(bmg, messageID, messageGroup, placeholderCount, &args);
    va_end(args);
}

void LytTextBox_c::ExtensionUserDataSetup() {
    dInfo_c *info = dInfo_c::getInstance();
    const nw4r::lyt::res::ExtUserData *userData = FindExtUserDataByName("shadow");
    if (userData != nullptr && *(int *) (((u8 *) userData) + userData->datOffs) != 0) {
        info->m_3da = true;
    }
}

void LytTextBox_c::setMessage(MsgRes_c *bmg, ulong messageID, ulong messageGroup, long placeholderCount, va_list *vargs) {
    nw4r::lyt::Size fontSize = GetFontSize();

    u8 fontIndex = bmg->getFont(messageID, messageGroup);
    SetFont(dFontMng_c::getFont(fontIndex));
    LytBase_c::s_TagPrc.mFontIndex = fontIndex;

    dInfo_c *info = dInfo_c::getInstance();
    info->m_3da = false;

    u16 extUserDataNum = GetExtUserDataNum();
    if (extUserDataNum != 0) {
        ExtensionUserDataSetup();
    }
    info->mTextBoxMessageID = messageID;
    info->mTextBoxMessageGroup = messageGroup;
    setText(bmg->getMsg(messageID, messageGroup), placeholderCount, vargs, bmg);

    float charWScale = bmg->getScale(messageID, messageGroup) * 0.01f;
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

void LytTextBox_c::setText(const wchar_t *text, long placeholderCount, ...) {
    va_list args;
    va_start(args, placeholderCount);
    setText(text, placeholderCount, &args, nullptr);
    va_end(args);
}

void LytTextBox_c::setText(const wchar_t *text, long placeholderCount, va_list *vargs, MsgRes_c *bmg) {
    static wchar_t Buffer[0x200];
    int count = 0;
    LytBase_c::s_TagPrc.preProcess(text, Buffer, ARRAY_SIZE(Buffer), &count, placeholderCount, vargs, bmg);
    SetString(Buffer, 0, count);
}
