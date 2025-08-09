#pragma once
#include <types.h>
#include <game/bases/d_2d/resource.hpp>
#include <game/mLib/m_2d.hpp>
#include <game/mLib/m_vec.hpp>

namespace d2d {

/// @brief Stores clipping settings for a layout.
/// @unofficial
struct ScissorMask {
    ScissorMask &operator=(const ScissorMask &other) {
        mPos = other.mPos;
        mSize = other.mSize;
        mEnabled = other.mEnabled;
        return *this;
    }

    nw4r::math::VEC2 mPos;
    nw4r::math::VEC2 mSize;
    bool mEnabled;
};

/// @brief A base 2D layout class.
class Multi_c : public m2d::Base_c {
public:
    Multi_c(); ///< Creates a layout.
    virtual ~Multi_c(); ///< Destroys the layout.
    virtual void draw(); ///< Draws the layout.
    virtual void calc(); ///< Applies the view matrix.

    /// @brief Builds the layout from a binary layout file.
    /// @param name The name of the binary layout file.
    /// @param resAcc The resource accessor to use. If nullptr, uses the internal accessor.
    /// @return Whether the layout was built successfully.
    virtual bool build(const char *name, ResAccMult_c *resAcc);

    void entry(); ///< Registers the layout to be drawn.
    void calcBefore(); ///< Calculates an animation step before the main calculation.
    void calcAfter(); ///< Calculates the view rectangle and view matrix after the main calculation.
    nw4r::lyt::Pane *getRootPane(); ///< Gets the root pane of the layout.
    nw4r::lyt::Pane *findPaneByName(const char *name); ///< Finds a pane by name.
    nw4r::lyt::TextBox *findTextBoxByName(const char *name); ///< Finds a text box by name.
    nw4r::lyt::Picture *findPictureByName(const char *name); ///< Finds a picture by name.
    nw4r::lyt::Window *findWindowByName(const char *name); ///< Finds a window by name.

private:
    m2d::Layout_c mLayout; ///< The layout instance.
    nw4r::lyt::DrawInfo mDrawInfo; ///< The parameters for drawing the layout.

public:
    ResAccMult_c *mpResAccessor; ///< The resource accessor for the layout.
    mVec2_c mPos; ///< The position of the layout.

    ScissorMask mScissorMask; ///< The scissor mask for the layout.

private:
    u32 mFlags; ///< The flags for the layout.
    u32 m_98; ///< @unused
};

} // namespace d2d
