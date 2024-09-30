#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>

/**
 * @brief Displays the controller information screen.
 * @ingroup bases
 * @details This is the screen that reads "Hold the Wii Remote sideways". It's spawned automatically by
 * the boot scene (dScBoot_c), which also performs the button input check and consequent
 * @ref STATE_e "state change".
 */
class dControllerInformation_c : public dBase_c {
public:

    /// @brief The identifier for each animation.
    enum ANIM_e {
        IN2BTN, ///< The button's pop-out animation.
        LOOP2BTN, ///< The button's blinking loop animation.
        HIT2BTN ///< The button's hit animation.
    };

    /// @brief The screen's current state.
    enum STATE_e {
        IDLE, ///< Initial state; waiting for layout creation.
        SHOW_IN, ///< Playing the button pop-out animation.
        SHOW_LOOP, ///< Waiting for the pop-out to finish.
        WAITING_FOR_END, ///< Playing the button animation while waiting for user input.
        END ///< Playing the button pressed sound effect and the button shrinking animation.
    };

    dControllerInformation_c(); ///< @copydoc fBase_c::fBase_c
    virtual ~dControllerInformation_c(); ///< @copydoc fBase_c::~fBase_c
    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();

    void setVisible(bool visible) { mVisible = visible; }
    void setState(STATE_e state) { mState = state; }

    /// @brief Loads the resources and creates the layout for the base.
    /// @return If the creation was successful.
private:
    bool createLayout();

    LytBase_c mLayout; ///< The layout of the base.
    bool mIsCreated; ///< If the layout has been successfully created.
    bool mVisible; ///< If the layout should be rendered.
    STATE_e mState; ///< Which state the base is currently in.
};
