#pragma once
#include <types.h>
/// @file

namespace nw4r {
namespace g3d {
namespace G3DState {

    /// @brief Specifies the states invalidated by the G3DState::Invalidate function.
    enum InvalidateFlag {
        INVALIDATE_TEXOBJ = 1, ///< Invalidates the texture object cache.
        INVALIDATE_TLUT = 2, ///< Invalidates the texture lookup table cache.
        INVALIDATE_TEV = 4, ///< Invalidates the TEV data cache.

        /// @brief Invalidates the cached number of TexCoords/channels/TEV stages/indirect stages
        /// and the cached cull mode.
        INVALIDATE_GENMODE2 = 8,
        INVALIDATE_CULLVTXDESC = 0x10, ///< Invalidates the vertex descriptor cache.
        INVALIDATE_CURRENTMTX = 0x20, ///< Invalidates the current matrix cache.
        INVALIDATE_TEXMTX = 0x40, ///< Invalidates the pre/post-conversion texture matrix cache.
        INVALIDATE_MISC = 0x80, ///< Invalidates the Z buffering state cache.
        INVALIDATE_FOG = 0x100, ///< Invalidates the fog cache.
        INVALIDATE_LIGHT = 0x200, ///< Invalidates the light cache.
        INVALIDATE_MTXARRAY = 0x400, ///< Invalidates the cached position coordinate and normal vector matrix arrays.

        /// @brief Invalidates all states.
        INVALIDATE_ALL = INVALIDATE_TEXOBJ | INVALIDATE_TLUT | INVALIDATE_TEV | INVALIDATE_GENMODE2
                        | INVALIDATE_CULLVTXDESC | INVALIDATE_CURRENTMTX | INVALIDATE_TEXMTX
                        | INVALIDATE_MISC | INVALIDATE_FOG | INVALIDATE_LIGHT | INVALIDATE_MTXARRAY
    };

    /// @brief Invalidates the state recorded within the G3D library, depending on the flag. See G3DState::InvalidateFlag.
    /// @details Must be used when calling GX functions outside of G3D.
    void Invalidate(ulong flag);

} // namespace G3DState
} // namespace g3d
} // namespace nw4r
