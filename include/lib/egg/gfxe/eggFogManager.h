#pragma once

#include <lib/nw4r/g3d/scn_root.hpp>

namespace EGG {

class FogManager {
public:
    FogManager(unsigned short);
    virtual void vf_8(); ///< @unofficial
    virtual void vf_c(); ///< @unofficial
    virtual void vf_10(); ///< @unofficial
    virtual void vf_14(); ///< @unofficial
    virtual ~FogManager();

    void Calc();
    void CopyToG3D(nw4r::g3d::ScnRoot *) const;

    u8 state;
    char pad[8];

    void DoneDraw() { state &= 0xfb; }
};

} // namespace EGG
