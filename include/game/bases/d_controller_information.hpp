#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>

/// @brief Displays the controller information screen.
/// @details This is the screen that reads "Hold the Wii Remote sideways".
/// @ingroup bases
class dControllerInformation_c : public dBase_c {
public:

    /// @brief The identifier for each animation.
    enum ANIM_e {
        IN2BTN, ///< The button's pop-out animation.
        LOOP2BTN, ///< The button's blinking loop animation.
        HIT2BTN ///< The button's hit animation.
    };

    /// @brief The state that controls what the layout should be doing.
    enum STATE_e {
        IDLE, ///< Initial state; waiting for layout creation.
        SHOW_IN, ///< Play the button pop-out animation.
        SHOW_LOOP, ///< Wait for the pop-out to finish.
        WAITING_FOR_END, ///< Play the button animation while waiting for user input.
        END ///< The button has been pressed, play the sound effect and the shrinking button animation.
    };

    dControllerInformation_c();
    virtual ~dControllerInformation_c();
    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();

    /// @brief Loads the resources and creates the layout for the base.
    /// @return If the creation was successful.
    bool createLayout();

    void setVisible(bool visible) { mVisible = visible; }
    void setState(STATE_e state) { mState = state; }

private:
    LytBase_c mLayout; ///< The layout of the base.
    bool mIsCreated; ///< If the layout has been successfully created.
    bool mVisible; ///< If the layout should be rendered.
    STATE_e mState; ///< Which state the base is currently in.
};
