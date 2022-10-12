#pragma once
#include <lib/nw4r/lyt/lyt_pane.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

class Window : public Pane {
    class Frame;

public:

    Window(const res::Window *, const ResBlockSet &);

    virtual int *GetRuntimeTypeInfo() const { return &typeInfo; }
    virtual void DrawSelf(const DrawInfo &);
    virtual void AnimateSelf(unsigned long);
    virtual ut::Color GetVtxColor(unsigned long) const;
    virtual ut::Color SetVtxColor(unsigned long, ut::Color);
    virtual unsigned char GetVtxColorElement(unsigned long) const;
    virtual void SetVtxColorElement(unsigned long, unsigned char);
    virtual Material *FindMaterialByName(const char *, bool);
    virtual void UnbindAnimationSelf(AnimTransform *);
    virtual bool GetMaterialNum() const;
    virtual Material *GetMaterial(unsigned long) const;

    Material *GetContentMaterial() const;
    Material *GetFrameMaterial(unsigned long) const;
    void DrawContent(const math::VEC2 &, const WindowFrameSize &, unsigned char);
    void DrawFrame(const math::VEC2 &, const Window::Frame &, const WindowFrameSize &, unsigned char);
    void DrawFrame4(const math::VEC2 &, const Window::Frame *, const WindowFrameSize &, unsigned char);
    void DrawFrame8(const math::VEC2 &, const Window::Frame *, const WindowFrameSize &, unsigned char);
    
private:
    static int typeInfo;
};

} // namespace lyt
} // namespace nw4r
