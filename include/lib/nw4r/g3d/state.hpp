#pragma once
#include <types.h>
/// @file

namespace nw4r {
namespace g3d {
namespace G3DState {

    /// @brief Specifies the states invalidated by the G3DState::Invalidate function.
    enum InvalidateFlag {
        INVALIDATE_TEXOBJ      = BIT_FLAG(0), ///< Invalidates the texture object cache.
        INVALIDATE_TLUT        = BIT_FLAG(1), ///< Invalidates the texture lookup table cache.
        INVALIDATE_TEV         = BIT_FLAG(2), ///< Invalidates the TEV data cache.

        /// @brief Invalidates the cached number of TexCoords/channels/TEV stages/indirect stages
        /// and the cached cull mode.
        INVALIDATE_GENMODE2    = BIT_FLAG(3),
        INVALIDATE_CULLVTXDESC = BIT_FLAG(4), ///< Invalidates the vertex descriptor cache.
        INVALIDATE_CURRENTMTX  = BIT_FLAG(5), ///< Invalidates the current matrix cache.
        INVALIDATE_TEXMTX      = BIT_FLAG(6), ///< Invalidates the pre/post-conversion texture matrix cache.
        INVALIDATE_MISC        = BIT_FLAG(7), ///< Invalidates the Z buffering state cache.
        INVALIDATE_FOG         = BIT_FLAG(8), ///< Invalidates the fog cache.
        INVALIDATE_LIGHT       = BIT_FLAG(9), ///< Invalidates the light cache.

        /// @brief Invalidates the cached position coordinate and normal vector matrix arrays.
        INVALIDATE_MTXARRAY    = BIT_FLAG(10),

        /// @brief Invalidates all states.
        INVALIDATE_ALL         = INVALIDATE_TEXOBJ | INVALIDATE_TLUT | INVALIDATE_TEV | INVALIDATE_GENMODE2
                                | INVALIDATE_CULLVTXDESC | INVALIDATE_CURRENTMTX | INVALIDATE_TEXMTX
                                | INVALIDATE_MISC | INVALIDATE_FOG | INVALIDATE_LIGHT | INVALIDATE_MTXARRAY
    };

    /// @brief Invalidates the state recorded within the G3D library, depending on the flag. See G3DState::InvalidateFlag.
    /// @details Must be used when calling GX functions outside of G3D.
    void Invalidate(ulong flag);

} // namespace G3DState
} // namespace g3d
} // namespace nw4r
