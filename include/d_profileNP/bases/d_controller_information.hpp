#pragma once
#include <dol/bases/d_base.hpp>
#include <dol/bases/d_lytbase.hpp>

/// @brief A base that display the controller information screen.
/// @details This is the screen that reads "Hold the Wii remote sideways".
class dControllerInformation_c : public dBase_c {
public:
    /// @brief The state that controls what the layout should be doing.
    enum STATE_e {
        IDLE, ///< Initial state; waiting for layout creation
        SHOW_IN, ///< Show the pop-out animation of the 2 button
        SHOW_LOOP, ///< Play the 2 button animation as soon as the pop-out animation has finished.
        WAITING_FOR_END, ///< Continue playing the 2 button animation.
        END ///< 2 button has been pressed, play the sound effect and play the shrinking 2 button animation.
    };

    dControllerInformation_c();
    virtual ~dControllerInformation_c();
    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    
    /// @brief Creates the layout for the base.
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
