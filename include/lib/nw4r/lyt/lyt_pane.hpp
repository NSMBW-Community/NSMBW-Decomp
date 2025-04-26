#pragma once
#include <lib/nw4r/lyt/lyt_draw_info.hpp>
#include <lib/nw4r/ut/color.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

class Pane {
public:

    Pane(const res::Pane *);

    virtual ~Pane();

    virtual int *GetRuntimeTypeInfo() const { return &typeInfo; }
    virtual void CalculateMtx(const DrawInfo &);
    virtual void Draw(const DrawInfo &);
    virtual void DrawSelf(const DrawInfo &);
    virtual void Animate(unsigned long);
    virtual void AnimateSelf(unsigned long);
    virtual ut::Color GetVtxColor(unsigned long) const;
    virtual ut::Color SetVtxColor(unsigned long, ut::Color);
    virtual unsigned char GetColorElement(unsigned long) const;
    virtual void SetColorElement(unsigned long, unsigned char);
    virtual unsigned char GetVtxColorElement(unsigned long) const;
    virtual void SetVtxColorElement(unsigned long, unsigned char);
    virtual Pane *FindPaneByName(const char *, bool);
    virtual Material *FindMaterialByName(const char *, bool);
    virtual void BindAnimation(AnimTransform *, bool, bool);
    virtual void UnbindAnimation(AnimTransform *, bool);
    virtual void UnbindAllAnimation(bool);
    virtual void UnbindAnimationSelf(AnimTransform *);
    virtual void AddAnimationLink(AnimationLink *);
    virtual ut::LinkList<AnimationLink, 0> *FindAnimationLinkSelf(AnimTransform *);
    virtual ut::LinkList<AnimationLink, 0> *FindAnimationLinkSelf(const AnimResource &);
    virtual void SetAnimationEnable(AnimTransform *, bool, bool);
    virtual void SetAnimationEnable(const AnimResource &, bool, bool);
    virtual bool GetMaterialNum() const;
    virtual Material *GetMaterial() const;
    virtual Material *GetMaterial(unsigned long) const;
    virtual void LoadMtx(const nw4r::lyt::DrawInfo &);

    void AppendChild(nw4r::lyt::Pane *);
    void RemoveChild(nw4r::lyt::Pane *);

    math::VEC2 GetVtxPos() const;
    unsigned short GetExtUserDataNum() const;
    void *FindExtUserDataByName(const char *);

    void setVisible(bool visible) {
        mFlags = (mFlags & 0xfe) | visible;
    }

    void setScale(const math::VEC2 &scale) { mScale = scale; }
    void setAlpha(u8 alpha) { mAlpha = alpha; }

public:
    char mFill1[0x28]; // To be RE'd
    math::VEC3 mPos;
    math::VEC3 mRot;
    math::VEC2 mScale;
private:
    float width;
    float height;
    char mFill2[0x64]; // To be RE'd
    u8 mAlpha;
    u8 mField_b9;
    u8 mOriginType;

public:
    u8 mFlags;

    static int typeInfo;
};

} // namespace lyt
} // namespace nw4r
