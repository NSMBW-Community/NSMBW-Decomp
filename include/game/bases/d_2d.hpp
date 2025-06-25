#pragma once
#include <game/bases/d_resource.hpp>
#include <game/mLib/m_2d/base.hpp>
#include <game/mLib/m_vec.hpp>
#include <lib/nw4r/lyt/lyt_layout.hpp>
#include <lib/nw4r/lyt/lyt_resource.hpp>
#include <lib/nw4r/lyt/lyt_draw_info.hpp>
#include <lib/nw4r/math/vec.hpp>

namespace d2d {

void init();
void draw();
void drawBefore();
void drawAfter();
void drawBtween(unsigned char, unsigned char);
int setAlpha_patrolPane_patrol(nw4r::lyt::Pane*, void*);

struct ClipSettings {
    ClipSettings() : mPos(0.0f, 0.0f), mSize(0.0f, 0.0f), mEnabled(false) {}

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
    virtual ~Multi_c();
    virtual void calc();
    virtual bool build(const char *, ResAccMult_c *);

    void entry();

private:
    nw4r::lyt::Layout mLayout;
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
