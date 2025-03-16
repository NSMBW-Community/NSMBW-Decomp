#pragma once
#include <game/bases/d_base.hpp>
#include <game/framework/f_profile.hpp>
#include <game/sLib/s_Phase.hpp>

/**
 * @brief The minimum required implementation for a scene base.
 * @ingroup bases
 * @details
 * ## Overview
 * Scene bases act as overall managers for specific parts of the game. As such, only one scene can be active at
 * any given time. All bases are created as children of the @ref ::m_nowScene "currently active scene", which
 * allows a scene switch to act as a garbage collection mechanism by deleting every other active base.
 *
 * Scene execution (and therefore every other base's) cannot begin until the scene has finished initializing.
 *
 * The game's initial scene is dScBoot_c (which performs various initialization tasks), while dScRestartCrsin_c
 * is used for soft resets to return to the title screen.
 *
 * ## Switching Scenes
 * To switch from the current scene to another, use the ::setNextScene function, providing the new scene's
 * profile name and parameters. The creation of the next scene is handled automatically.
 *
 * @note
 * - Scene switches during resets are typically disallowed. However, this restriction can be bypassed by setting
 * the @p forceChange parameter to @p true in the ::setNextScene call.
 * - A scene switch cannot occur until the existing scene has completed its initialization, regardless of the
 * aforementioned parameter.
 * - Consecutive calls to set the next scene will have no effect.
 *
 * ## Scene Initialization
 * An @ref mpPhase "initialization phase" can be provided to set up the scene step by step. Typical actions
 * include:
 * - Loading additional resources (such as @ref dRes_c "archives", @ref LytBase_c "layouts" or effects) not
 * linked to a specific base (e.g. the worldmap model in the @ref dScWMap_c "worldmap scene"). Checks must be
 * added to ensure these resources have finished loading before initialization ends.
 * - Loading additional sound objects (see [below](#audio-initialization)).
 * - Creating specific bases necessary for the scene's functionality (e.g. the game interface for the scene).
 * @note The LASTACTOR base is created automatically.
 * - Setting the scene's paint function, which must always be the final step in initialization.
 * - In earlier development stages, any base used by the scene was dynamically linked in the first
 * initialization step.
 *
 * Failure to complete initialization (e.g. due to missing resources) will cause the game to softlock.
 *
 * @hint{Existing scene initialization phases are a good reference point for implementing custom scenes.}
 *
 * ### Audio Initialization
 * The following sound objects are automatically created and deleted; other sound objects must be managed
 * manually:
 * - SndObjctCmnEmy_c
 * - SndObjctCmnMap_c
 * - NonPosSndObjctAmb_c
 *
 * @todo Expand this section when dAudio and the related classes are decompiled.
 *
 * ## Scene Fading
 * The fader to be used for a scene transition can be set with dFader_c::setFader. The fade-in and fade-out
 * durations can be set using ::setFadeInFrame and ::setFadeOutFrame respectively. Both can be set simultaneously
 * with ::setFadeInOutFrame).
 *
 * The previous scene is automatically deleted once the fade-out completes. The next scene will fade in automatically
 * unless ::m_isAutoFadeIn is set to @p false (in that case, the fade-in must be started manually by calling
 * dFader_c::startFadeIn and audio must be reinitialized with dAudio::requestStartScene).
 */
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
