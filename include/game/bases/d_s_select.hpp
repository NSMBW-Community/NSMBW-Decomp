#pragma once
#include <game/bases/d_scene.hpp>

/// @brief An empty scene that does nothing.
/// @details Presumably, it was used as a debug level selection scene.
/// @ingroup bases
/// @unused
class dScSelect_c : public dScene_c {
public:
    dScSelect_c() {} ///< @copydoc dScene_c::dScene_c
    virtual ~dScSelect_c() {} ///< @copydoc dScene_c::~dScene_c
};
