// https://raw.githubusercontent.com/doldecomp/ogws/0d988270c9009a2983b888c2b75b5430a35f23e4/include/nw4r/lyt/lyt_common.h

#pragma once

namespace nw4r {
namespace lyt {

/// @brief The vertex color index for each corner of a pane.
enum VertexColor {
    VERTEXCOLOR_LT, ///< The vertex color of the top left corner.
    VERTEXCOLOR_RT, ///< The vertex color of the top right corner.
    VERTEXCOLOR_LB, ///< The vertex color of the bottom left corner.
    VERTEXCOLOR_RB, ///< The vertex color of the bottom right corner.
    VERTEXCOLOR_MAX
};

} // namespace lyt
} // namespace nw4r
