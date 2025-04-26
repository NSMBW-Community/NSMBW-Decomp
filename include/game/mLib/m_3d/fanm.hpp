#pragma once
#include <game/mLib/m_3d/banm.hpp>

namespace m3d {

    /// @brief Animation object.
    class fanm_c : public banm_c {
    public:
        fanm_c(); ///< Constructs an animation object.
        virtual ~fanm_c(); ///< Destroys the animation object.

        /// @brief Updates the animation.
        /// Call this function every frame to update the animation.
        virtual void play();

        /**
         * @brief Starts the animation with the given parameters.
         *
         * @param duration The number of frames in the animation.
         * @param playMode The play mode of the animation.
         * @param updateRate The speed of the animation.
         * @param startFrame The starting frame of the animation. Set to -1 to start from the beginning.
         */
        void set(float duration, m3d::playMode_e playMode, float updateRate, float startFrame);

        /// @brief Jumps to the specified frame in the animation.
        /// @param frame The frame to jump to.
        void setFrame(float frame);

        /// @brief Checks whether the animation is stopped.
        bool isStop() const;

        /// @brief Checks whether the animation has reached the specified frame.
        /// @param frame The frame to check.
        bool checkFrame(float frame) const;

        float mFrameMax; ///< The last frame number of the animation.
        float mFrameStart; ///< The first frame number of the animation.
        float mCurrFrame; ///< The frame the animation is currently on.
        u8 mPlayMode; ///< The play mode of the animation.
    };
}
