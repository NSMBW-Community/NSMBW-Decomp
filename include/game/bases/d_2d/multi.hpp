#pragma once
#include <types.h>
#include <game/bases/d_2d/resource.hpp>
#include <game/mLib/m_2d.hpp>
#include <game/mLib/m_vec.hpp>

namespace d2d {

/// @unofficial
struct ClipSettings {
    ClipSettings() {}
    ClipSettings(const mVec2_c &pos, const mVec2_c &size) : mPos(pos), mSize(size) {
        mEnabled = true;
    }
    void setPos(const mVec2_c &pos) { mPos = pos; }
    void setSize(const mVec2_c &size) { mSize = size; }
    void set(const mVec2_c &pos, const mVec2_c &size) {
        mPos = pos;
        mSize = size;
    }
    void enable() { mEnabled = true; }

    ClipSettings &operator=(const ClipSettings &other) {
        mPos = other.mPos;
        mSize = other.mSize;
        mEnabled = other.mEnabled;
        return *this;
    }

    mVec2_c mPos;
    mVec2_c mSize;
    bool mEnabled;
};

class Multi_c : public m2d::Base_c {
public:
    Multi_c();
    virtual ~Multi_c();
    virtual void draw();
    virtual void calc();
    virtual bool build(const char *name, ResAccMult_c *resAcc);

    void entry();
    void calcBefore();
    void calcAfter();
    nw4r::lyt::Pane *getRootPane();
    nw4r::lyt::Pane *findPaneByName(const char *name);
    nw4r::lyt::TextBox *findTextBoxByName(const char *name);
    nw4r::lyt::Picture *findPictureByName(const char *name);
    nw4r::lyt::Window *findWindowByName(const char *name);

protected:
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
