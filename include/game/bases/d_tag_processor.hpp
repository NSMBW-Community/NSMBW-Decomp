#pragma once

#include <cstdarg>
#include <game/bases/d_message.hpp>
#include <nw4r/ut.h>

class TagProcessor_c : public nw4r::ut::WideTagProcessor {
public:
    TagProcessor_c();

    u8 mPad[0xc0];
    u8 mFontIndex;

    void preProcess(
        const wchar_t *text, wchar_t *buf,
        unsigned long bufLen, int *writeLen,
        long param, va_list *vargs,
        MsgRes_c *bmg
    );

    static bool isZeroWidthSpace; ///< @unofficial
};
