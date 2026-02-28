#pragma once

#include <types.h>

namespace Util {

/// @brief A bidirectional frame counter.
/// @ingroup bases
class FrameCounter_c {
public:
    /// @brief The possible behaviors after the frame counter is finished.
    enum Type_e {
        TYPE_ONETIME,     ///< Locks the frame to the target frame.
        TYPE_LOOP,        ///< Continues counting in the same direction.
        TYPE_OSCILLATING, ///< Continues counting in the opposite direction.
    };

    FrameCounter_c();  ///< Constructs a new frame counter.
    ~FrameCounter_c(); ///< Destroys the frame counter.

    /// @brief Initializes the frame interval.
    /// @details Sets the current frame to the start frame.
    /// @param startFrame The start frame.
    /// @param endFrame The end frame.
    void init(f32 startFrame, f32 endFrame);

    /// @brief Fully initializes into a valid state.
    /// @details Sets the current frame to the start frame.
    /// @param updateRate The update rate.
    /// @param type The play type.
    /// @param startFrame The start frame.
    /// @param endFrame The end frame.
    void play(f32 updateRate, Type_e type, f32 startFrame, f32 endFrame);

    /// @brief Partially initializes into a valid state without initializing the frame interval.
    /// @details Sets the current frame to the start frame.
    /// @param updateRate The update rate.
    /// @param type The play type.
    void play(f32 updateRate, Type_e type);

    /// @brief Updates the frame counter.
    /// @details Sets finished to @p true if the counter finishes.
    void calc();

    /// @brief Gets the current frame.
    /// @return The current frame.
    f32 getFrame() const { return m_frame; }

    /// @brief Sets the current frame.
    /// @param frame The new current frame.
    void setFrame(f32 frame) { m_frame = frame; }

    /// @brief Gets the update rate.
    /// @return The update rate.
    f32 getUpdateRate() const { return m_updateRate; }

    /// @brief Sets the update rate.
    /// @param updateRate The new update rate.
    void setUpdateRate(f32 updateRate) { m_updateRate = updateRate; }

    /// @brief Gets whether the counter is finished.
    /// @return @p true the counter is finished, otherwise @p false.
    bool getFinished() const { return m_finished; }

private:
    /// @brief The current frame in the interval [start, end].
    /// @details Initialized to the start frame by #init and/or #play.
    /// Updated based on the update rate and type behavior in #calc.
    /// Can optionally be set manually with #setFrame.
    f32 m_frame;

    /// @brief Lower bound of the frame interval.
    /// @details Initialized by #init and/or #play.
    f32 m_startFrame;

    /// @brief Upper bound of the frame interval.
    /// @details Initialized by #init and/or #play.
    f32 m_endFrame;

    /// @brief Controls the direction and rate at which the frame updates.
    /// @details If the update rate is positive, the target frame will be the end frame.
    /// If the update rate is negative, the target frame will be the start frame.
    /// Initialized by #play. Can optionally be set manually with #setUpdateRate.
    f32 m_updateRate;

    /// @brief Whether or not the counter is finished.
    /// @details Behavior is contingent on #m_type.
    /// - #TYPE_ONETIME: the counter will be finished until there is a modified update rate or frame.
    /// - #TYPE_LOOP and #TYPE_OSCILLATING: the counter will be finished only when the change occurs.
    bool m_finished;

    /// @brief Controls what happens to the frame when it reaches the target frame.
    Type_e m_type;
};

} // namespace Util
