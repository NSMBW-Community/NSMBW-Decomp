#pragma once
#include <game/bases/d_base.hpp>
#include <game/framework/f_profile.hpp>
#include <game/sLib/s_Phase.hpp>

/// @brief The minimum required implementation for a scene base.
/// @ingroup bases
class dScene_c : public dBase_c {
public:
    dScene_c(); ///< Creates a new scene.
    ~dScene_c(); ///< Destroys the scene.

    virtual int preCreate();

    /// @copybrief fBase_c::postCreate
    /// @details Appends a LASTACTOR child base to this scene.
    virtual void postCreate(fBase_c::MAIN_STATE_e status);

    virtual int preDelete();
    virtual void postDelete(fBase_c::MAIN_STATE_e status);

    /// @copybrief fBase_c::preExecute
    /// @details If ::m_nextScene is set, prepares to transition to the next scene.
    /// Once all initial child bases have been added, the execute and draw operations are enabled on this base
    /// and its children.
    virtual int preExecute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);

    virtual int preDraw();
    virtual void postDraw(fBase_c::MAIN_STATE_e status);

    static void setStartScene(); ///< Sets up the scene to be shown when the game boots up.
    static void setResetScene(); ///< Sets up the scene to be shown after a game reset.
    static dScene_c *createNextScene(); ///< Creates and returns a root base for the next scene.

    /// @brief Attempts to prepare the transition to a new scene.
    /// @details If the game is resetting, the transition is not performed unless forceChange is @p true.
    /// @param nextScene The next scene's profile name.
    /// @param param The next scene's parameters.
    /// @param forceChange If the transition should always be performed.
    static void setNextScene(ProfileName nextScene, unsigned long param, bool forceChange);

    /// @brief Sets the duration of the next fade-in transition to @p length.
    static void setFadeInFrame(unsigned short length);

    /// @brief Sets the duration of the next fade-out transition to @p length.
    static void setFadeOutFrame(unsigned short length);

    /// @brief Sets the duration of the next fade-in and fade-out transitions to @p length.
    static void setFadeInOutFrame(unsigned short length);

    sPhase_c *mpPhase; ///< The phase used for scene initialization.

    static u32 mPara; ///< The parameters for the next scene.
    static ProfileName m_nextScene; ///< The profile name of the next scene.
    static ProfileName m_nowScene; ///< The profile name of the current scene.
    static ProfileName m_oldScene; ///< The profile name of the previous scene.
    static bool m_otherSceneFlg; ///< Whether the next scene has already been created.

    static u16 m_fadeInFrame; ///< The duration of the next fade-in.
    static u16 m_fadeOutFrame; ///< The duration of the next fade-out.
    static bool m_isAutoFadeIn; ///< If a fade-in should automatically be performed on scene load.
};
