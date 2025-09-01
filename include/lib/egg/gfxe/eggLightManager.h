#pragma once

#include <lib/egg/gfxe/eggLightTextureManager.h>
#include <nw4r/math.h>
#include <nw4r/g3d.h>

namespace EGG {

class LightManager {
public:
    LightManager(unsigned long, unsigned long, unsigned char);
    virtual void vf_8(); ///< @unofficial
    virtual void vf_c(); ///< @unofficial
    virtual void vf_10(); ///< @unofficial
    virtual void vf_14(); ///< @unofficial
    virtual ~LightManager();
    virtual void vf_18(); ///< @unofficial
    virtual void Calc(nw4r::g3d::ScnRoot *);
    virtual void CalcView(const nw4r::math::MTX34 &, unsigned char, nw4r::g3d::ScnRoot *);
    virtual void DoneDraw();

    char pad[0x10];
    LightTextureManager *ltMgr;
    char pad2[0x10];
};

} // namespace EGG
