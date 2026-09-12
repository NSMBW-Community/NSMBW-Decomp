#pragma once

#include <egg/core/eggHeap.h>
#include <egg/core/eggMsgRes.h>

/// @brief A simple wrapper around EGG::MsgRes.
/// @ingroup bases
class MsgRes_c : public EGG::MsgRes {
public:
    MsgRes_c(const void *p, EGG::Heap *heap); ///< Constructs a new message resource.
    virtual ~MsgRes_c(); ///< Destroys the @p MsgRes_c instance.

    u8 getFont(ulong messageGroup, ulong messageID); ///< Gets the font for a message.
    u16 getScale(ulong messageGroup, ulong messageID); ///< Gets the scale for a message.
};
