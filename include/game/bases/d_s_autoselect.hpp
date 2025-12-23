#pragma once
#include <game/bases/d_scene.hpp>

/**
 * @brief An empty scene that does nothing.
 * @details Presumably, it was used to automatically cycle between stages, which was reimplemented
 * in the Shield version as the @p AUTO_PILOTING .INI configuration.
 * @ingroup bases
 * @unused
 */
class dScAutoSelect_c : public dScene_c {
public:
    dScAutoSelect_c() {} ///< @copydoc dScene_c::dScene_c
    virtual ~dScAutoSelect_c() {} ///< @copydoc dScene_c::~dScene_c
};
