#pragma once

#include <lib/egg/gx/eggLightTextureManager.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <lib/nw4r/g3d/scn_root.hpp>

namespace EGG {

class LightManager {
public:
    LightManager(unsigned long, unsigned long, unsigned char);
    virtual void virtualpad0();
    virtual void virtualpad1();
    virtual void virtualpad2();
    virtual void virtualpad3();
    virtual ~LightManager();
    virtual void virtualpad4();
    virtual void Calc(nw4r::g3d::ScnRoot *);
    virtual void CalcView(const nw4r::math::MTX34 &, unsigned char, nw4r::g3d::ScnRoot *);
    virtual void DoneDraw();

    char pad[0x10];
    LightTextureManager *ltMgr;
    char pad2[0x10];
};

} // namespace EGG
