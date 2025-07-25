#pragma once
#include <lib/nw4r/lyt/lyt_types.hpp>
#include <lib/nw4r/lyt/lyt_animation.hpp>
#include <lib/nw4r/lyt/lyt_draw_info.hpp>
#include <lib/nw4r/ut/color.hpp>
#include <lib/nw4r/ut/list.hpp>
#include <lib/nw4r/ut/RuntimeTypeInfo.hpp>
#include <lib/nw4r/math/vec.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

namespace detail {

    class PaneBase {
    public:
        inline PaneBase() : mLink() {}
        virtual ~PaneBase();
        nw4r::ut::LinkListNode mLink;
    };

} // namespace detail

class Pane : detail::PaneBase {
private:
    enum FlagBit { BIT_VISIBLE, BIT_INFLUENCED_ALPHA, BIT_LOCATION_ADJUST };

    typedef ut::LinkList<Pane, 4> PaneList;

public:

    Pane(const res::Pane *);

    virtual ~Pane();

    NW4R_UT_RUNTIME_TYPEINFO;

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
        detail::SetBit(&mFlag, BIT_VISIBLE, visible);
    }

    void setScale(const math::VEC2 &scale) { mScale = scale; }
    void setAlpha(u8 alpha) { mAlpha = alpha; }
    void setGlbAlpha(u8 glbAlpha) { mGlbAlpha = glbAlpha; }

public:
    Pane* mpParent;
    PaneList mChildList;
    AnimationLinkList mAnimList;
    Material* mpMaterial;
    math::VEC3 mTranslate;
    math::VEC3 mRotate;
    math::VEC2 mScale;
    Size mSize;
    math::MTX34 mMtx;
    math::MTX34 mGlbMtx;
    char mName[0x10];
    char mUserData[8];
    u8 mAlpha;
    u8 mGlbAlpha;
    u8 mBasePosition;
    u8 mFlag;
    u8 mbUserAllocated;
};

} // namespace lyt
} // namespace nw4r
