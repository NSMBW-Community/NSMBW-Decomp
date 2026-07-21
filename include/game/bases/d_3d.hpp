#pragma once

#include <game/mLib/m_3d.hpp>

namespace d3d {
    void setNodeVisibility(m3d::bmdl_c *, int, int);
    void resetTevColorAll(m3d::bmdl_c *, _GXTevRegID, bool);
    void resetTevKColorAll(m3d::bmdl_c *, _GXTevKColorID, bool);
};
