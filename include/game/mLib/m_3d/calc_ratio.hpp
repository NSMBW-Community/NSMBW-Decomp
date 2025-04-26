#pragma once

namespace m3d {
    /**
     * @brief Class to smoothly blend between two values.
     *
     * This interpolation is done using a custom interpolation.
     * The mWeight follows an ease-in/ease-out curve and the difference
     * between the previous and current value of this weight is used
     * to compute the actual interpolation.
     */
    class calcRatio_c {
    public:
        calcRatio_c(); ///< Constructs a ratio calculator.
        virtual ~calcRatio_c() {} ///< Destroys the ratio calculator.

        bool isEnd() const; ///< Returns whether the blend is complete.
        void calc(); ///< Advances the blend by one time step.
        void set(float duration); ///< Starts a blend with a given duration.
        void remove(); ///< Cancels and resets the blend.
        void reset(); ///< Resets the blend to its initial state.
        void offUpdate(); ///< Pauses the blend.

        float getScaleFrom() { return mScaleFrom; }
        float getScaleTo() { return mScaleTo; }
        float getSlerpParam() { return mInterpolateT; }

        bool isActive() const { return mIsActive; }
        bool isBlending() const { return mIsBlending; }

    private:
        float mWeight; ///< Current weight of the blend.
        float mT; ///< The current time of the blend, between 0 and 1.
        float mTimeStep; ///< How much to advance mT by each iteration.

        float mScaleFrom; ///< How much to multiply the old value by (between 0 and 1).
        float mScaleTo; ///< How much to multiply the new value by (between 0 and 1).
        float mInterpolateT; ///< If using an interpolation function, use this as the @p t parameter.

        bool mIsActive; ///< Whether the blend is paused.
        bool mIsBlending; ///< Whether the blend is active.
    };
}
