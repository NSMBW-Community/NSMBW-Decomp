#pragma once
#include <game/mLib/m_fader_base.hpp>

/// @brief A fader that wipes a texture in and out to/from the center of the screen.
/// @ingroup mlib
class mWipeFader_c : public mFaderBase_c {
public:

    mWipeFader_c(mColor col, mFaderBase_c::EStatus status);

    virtual ~mWipeFader_c();
    virtual void setStatus(EStatus status);
    virtual int calc();
    virtual void draw();
    virtual void setTexture(void *data, int width, int height); ///< Sets the texture to use for the fader.

private:
    /// @brief Calculates the texture matrix needed for drawing the texture to the screen.
    /// @details Automatically called by calc().
    virtual void calcMtx();

    Mtx mTexMtx; ///< The texture matrix.
    u8 mProgress; ///< A value from 0 to 255 that represents the current progress of the fader.
    float mAspectRatioFactor; ///< How much to stretch the texture horizontally to compensate for the screen's aspect ratio.
    void *mpTextureData; ///< The texture data.
    int mTexWidth; ///< The width of the texture.
    int mTexHeight; ///< The height of the texture.
};
