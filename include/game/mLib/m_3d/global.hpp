#pragma once

#include <game/mLib/m_allocator.hpp>
#include <lib/egg/gfxe/eggLightManager.h>
#include <lib/egg/gfxe/eggFogManager.h>
#include <lib/egg/gfxe/eggScreen.h>
#include <nw4r/g3d.h>

/// @addtogroup mlib
/// @{

/// @brief mLib 3D library
namespace m3d {
    class bmdl_c;

    void initHio(const char *name);

    bool create(EGG::Heap *heap, ulong maxChildren, ulong maxScnObj, ulong numLightObj, ulong numLightSet);
    bool create(EGG::Heap *heap, GXPixelFmt fmt, GXColor col, ulong maxChildren, ulong maxScnObj, ulong numLightObj,
                ulong numLightSet, ulong numLightMgr, ulong numFogMgr);

    bool createLightMgr(EGG::Heap *heap, u16 lmArg1, u16 lmArg2, u8 lmArg3, bool createFogMgr, int idx);
    void removeLightMgr(int idx);

    bool createFogMgr(EGG::Heap *heap, int fmArg, int idx);
    void removeFogMgr(int idx);

    nw4r::g3d::ScnRoot *getScnRoot();
    nw4r::g3d::Camera getCamera(int idx);
    nw4r::g3d::Camera getCurrentCamera();
    int getCurrentCameraID();
    void setCurrentCamera(int idx);

    nw4r::g3d::LightSetting *getLightSettingP();
    EGG::LightManager *getLightMgr(int idx);
    EGG::FogManager *getFogMgr(int idx);

    void drawDone(int idx);

    void drawLightMapTexture(int idx);
    bool replaceLightMapTexture(char const *name, m3d::bmdl_c &mdl, int idx);

    void calcWorld(int idx);
    void calcMaterial();
    void calcView(int idx);

    void drawOpa();
    void drawXlu();

    void pushBack(nw4r::g3d::ScnObj *obj);

    void clear();
    void reset();

    int getNodeID(nw4r::g3d::ResMdl mdl, char const *name);

    void resetMaterial(); ///< Turns off all indirect texture processing.

    void screenEffectReset(int cameraID, EGG::Screen &screen);
}
/// @}
