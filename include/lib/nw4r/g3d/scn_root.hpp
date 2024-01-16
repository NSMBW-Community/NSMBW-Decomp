#pragma once

#include <lib/nw4r/g3d/camera.hpp>
#include <lib/nw4r/g3d/scn_group.hpp>

namespace nw4r {
namespace g3d {

class LightSetting {
    u32 pad;
};

class ScnRoot : public ScnGroup {
public:
    static ScnRoot *Construct( MEMAllocator*, unsigned long*, unsigned long, unsigned long, unsigned long, unsigned long );

    Camera GetCamera(int);
    Camera GetCurrentCamera();
    void SetCurrentCamera(int);
    void CalcWorld();
    void CalcMaterial();
    void CalcView();
    void GatherDrawScnObj();
    void ZSort();
    void DrawOpa();
    void DrawXlu();

    void *gather;
    u32 unk1;
    u32 unk2;
    u8 currCameraID;
    char cameraData[0x2180];
    char fogData[0x600];
    LightSetting lightSetting;
    u32 unk3;

    NW4R_G3D_TYPE_OBJ_DECL(ScnRoot);
};

} // namespace g3d
} // namespace nw4r
