#pragma once
#include <types.h>
#include <game/bases/d_2d/resource.hpp>
#include <game/mLib/m_2d.hpp>
#include <game/mLib/m_vec.hpp>

namespace d2d {

/// @unofficial
struct ClipSettings {
    ClipSettings &operator=(const ClipSettings &other) {
        mPos = other.mPos;
        mSize = other.mSize;
        mEnabled = other.mEnabled;
        return *this;
    }

    nw4r::math::VEC2 mPos;
    nw4r::math::VEC2 mSize;
    bool mEnabled;
};

class Multi_c : public m2d::Base_c {
public:
    Multi_c();
    virtual ~Multi_c();
    virtual void draw();
    virtual void calc();
    virtual bool build(const char *, ResAccMult_c *);

    void entry();
    void calcBefore();
    void calcAfter();
    nw4r::lyt::Pane *getRootPane();
    nw4r::lyt::Pane *findPaneByName(const char *name);
    nw4r::lyt::TextBox *findTextBoxByName(const char *name);
    nw4r::lyt::Picture *findPictureByName(const char *name);
    nw4r::lyt::Window *findWindowByName(const char *name);

private:
    m2d::Layout_c mLayout;
    nw4r::lyt::DrawInfo mDrawInfo;

public:
    ResAccMult_c *mpResAccessor;
    mVec2_c mPos;

    ClipSettings mClipSettings;

private:
    u32 mFlags;
    u32 mUnknown_98;
};

} // namespace d2d
