#pragma once

#include <lib/nw4r/g3d/scn_root.hpp>

namespace EGG {

class FogManager {
public:
    FogManager(unsigned short);
    virtual void virtualpad0();
    virtual void virtualpad1();
    virtual void virtualpad2();
    virtual void virtualpad3();
    virtual ~FogManager();

    void Calc();
    void CopyToG3D(nw4r::g3d::ScnRoot *) const;

    u8 state;
    char pad[8];

    void DoneDraw() { state &= 0xfb; }
};

} // namespace EGG
