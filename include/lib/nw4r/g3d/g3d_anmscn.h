#ifndef NW4R_G3D_ANM_SCN_H
#define NW4R_G3D_ANM_SCN_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_anmobj.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_fog.h>
#include <nw4r/g3d/g3d_light.h>
#include <nw4r/g3d/g3d_obj.h>

namespace nw4r {
namespace g3d {

// Forward declarations
struct AmbLightAnmResult;
class AnmScnRes;
struct CameraAnmResult;
struct FogAnmResult;
struct LightAnmResult;
class LightSetting;

class AnmScn : public G3dObj {
public:
    explicit AnmScn(MEMAllocator* pAllocator);
    virtual ~AnmScn(); // at 0x10

    virtual void SetFrame(f32 frame) = 0; // at 0x1C
    virtual f32 GetFrame() const = 0;     // at 0x20

    virtual void SetUpdateRate(f32 rate) = 0; // at 0x24
    virtual f32 GetUpdateRate() const = 0;    // at 0x28
    virtual void UpdateFrame() = 0;           // at 0x2C

    virtual AnmScnRes* Attach(int idx, AnmScnRes* pRes); // at 0x30
    virtual AnmScnRes* Detach();                         // at 0x34

    virtual ulong GetNumLightSet() const = 0;      // at 0x38
    virtual ulong GetNumAmbLight() const = 0;      // at 0x3C
    virtual ulong GetNumDiffuseLight() const = 0;  // at 0x40
    virtual ulong GetNumSpecularLight() const = 0; // at 0x44
    virtual ulong GetNumFog() const = 0;           // at 0x48
    virtual ulong GetNumCamera() const = 0;        // at 0x4C

    virtual ulong GetLightSetMaxRefNumber() const = 0;     // at 0x50
    virtual ulong GetAmbLightMaxRefNumber() const = 0;     // at 0x54
    virtual ulong GetDiffuseLightMaxRefNumber() const = 0; // at 0x58
    virtual ulong GetFogMaxRefNumber() const = 0;          // at 0x5C
    virtual ulong GetCameraMaxRefNumber() const = 0;       // at 0x60

    virtual bool GetLightSet(LightSet set, ulong refNumber) = 0; // at 0x64
    virtual ut::Color GetAmbLightColor(ulong refNumber) = 0;     // at 0x68
    virtual void GetLight(LightObj* pDiff, LightObj* pSpec,
                          ulong refNumber) = 0;                 // at 0x6C
    virtual void GetFog(Fog fog, ulong refNumber) = 0;          // at 0x70
    virtual bool GetCamera(Camera camera, ulong refNumber) = 0; // at 0x74

    virtual ulong GetSpecularLightID(ulong refNumber) const = 0; // at 0x78
    virtual bool HasSpecularLight(ulong refNumber) const = 0;  // at 0x7C

    virtual AmbLightAnmResult* GetAmbLightResult(AmbLightAnmResult* pResult,
                                                 ulong refNumber) = 0; // at 0x80
    virtual LightAnmResult* GetLightResult(LightAnmResult* pResult,
                                           ulong refNumber) = 0; // at 0x84
    virtual FogAnmResult* GetFogResult(FogAnmResult* pResult,
                                       ulong refNumber) = 0; // at 0x88
    virtual CameraAnmResult* GetCameraResult(CameraAnmResult* pResult,
                                             ulong refNumber) = 0; // at 0x8C

    void GetLightSetting(LightSetting* pSetting);

private:
    NW4R_G3D_RTTI_DECL_DERIVED(AnmScn, G3dObj);
};

} // namespace g3d
} // namespace nw4r

#endif
