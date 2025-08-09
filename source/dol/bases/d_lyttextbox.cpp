#include <cstdarg>
#include <game/bases/d_lyttextBox.hpp>
#include <game/bases/d_font_manager.hpp>
#include <game/bases/d_info.hpp>

void LytTextBox_c::setMessage(MsgRes_c *bmg, ulong messageID, ulong messageGroup, long param, ...) {
    dInfo_c *info = dInfo_c::getInstance();
    info->field_3d0 = messageID;
    info->field_3d4 = messageGroup;

    va_list args;
    va_start(args, param);
    setMessage(bmg, messageID, messageGroup, param, &args);
    va_end(args);
}

void LytTextBox_c::ExtensionUserDataSetup() {
    dInfo_c *info = dInfo_c::getInstance();
    const nw4r::lyt::res::ExtUserData *userData = FindExtUserDataByName("shadow");
    if (userData != nullptr && *(int *) (((u8 *) userData) + userData->datOffs) != 0) {
        info->field_3da = true;
    }
}

void LytTextBox_c::setMessage(MsgRes_c *bmg, ulong messageID, ulong messageGroup, long param, va_list *vargs) {
    dInfo_c *info = dInfo_c::getInstance();
    SetFont(dFontMng_c::getFont(bmg->getFont(messageID, messageGroup)));
    wchar_t Buffer[100];
    Buffer[99] = L'\0';
    info->field_3da = 0;
    u16 extUserDataNum = GetExtUserDataNum();
    if (extUserDataNum != 0) {
        ExtensionUserDataSetup();
    }
    info->field_3d0 = messageID;
    info->field_3d4 = messageGroup;
    setText(bmg->getMsg(messageID, messageGroup), param, vargs, bmg);
    float fScale = bmg->getScale(messageID, messageGroup) * 0.01;
    SetFontSize(nw4r::lyt::Size(fScale * GetFont()->GetWidth(), fScale * GetFont()->GetHeight()));
    nw4r::ut::WideTextWriter textWriter;
    nw4r::ut::Rect rect = GetTextDrawRect(&textWriter);
}
