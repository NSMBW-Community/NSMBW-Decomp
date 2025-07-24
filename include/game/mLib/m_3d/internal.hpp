#pragma once
#include <game/mLib/m_allocator.hpp>
#include <lib/egg/gfxe/eggLightManager.h>
#include <lib/egg/gfxe/eggFogManager.h>

namespace m3d {
    class internal {
    public:
        static mAllocator_c *l_allocator_p;
        static nw4r::g3d::ScnRoot *l_scnRoot_p;
        static unsigned int l_numLightMgr;
        static EGG::LightManager **l_lightMgr_pp;
        static unsigned int l_numFogMgr;
        static EGG::FogManager **l_fogMgr_pp;
    };
}
