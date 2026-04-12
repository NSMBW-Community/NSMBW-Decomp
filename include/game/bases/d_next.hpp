#pragma once

#include <game/bases/d_fader.hpp>
#include <game/bases/d_cd_data.hpp>
#include <lib/egg/core/eggHeap.h>

/// @brief Manager for scene changes triggered by entrances.
class dNext_c {
public:
    /// @brief The different types of scene changes.
    /// @unofficial
    enum SceneChangeType_e {
        NEXT_DIFFERENT_FILE, ///< The next entrance is in a different file.
        NEXT_DIFFERENT_AREA, ///< The next entrance is in a different area of the same file.
        NEXT_SAME_AREA ///< The next entrance is in the same area.
    };

    dNext_c(); ///< Constructs a new class instance and sets #m_instance to it.

    void initialize(); ///< Resets the class to its initial state.
    void execute(); ///< Checks for scene changes and executes them when necessary.

    /// @brief Updates the saved entrance data for the next scene change if not already set.
    /// @param fileIdx The file index of the entrance to set.
    /// @param nextGoto The entrance index of the entrance to set.
    /// @param faderType The type of fader to use for the scene change.
    void setChangeSceneNextDat(u8 fileIdx, u8 nextGoto, dFader_c::fader_type_e faderType);

    /// @brief Sets the entrance data for the next scene change.
    /// @param fileIdx The file index of the entrance to set.
    /// @param nextGoto The entrance index of the entrance to set.
    void setOwnNextData(u8 fileIdx, u8 nextGoto);

    /// @brief Returns whether the currently saved entrance is different from the given one.
    /// @param fileIdx The file index of the entrance to compare to.
    /// @param nextGoto The entrance index of the entrance to compare to.
    /// @return Whether the currently saved entrance is different from the given one.
    /// @unofficial
    bool isNextDifferent(u8 fileIdx, u8 nextGoto);

    /// @brief Search for a next entrance at the given coordinates in the given file.
    /// @param fileIdx The file to search in.
    /// @param x The x coordinate to search at.
    /// @param y The y coordinate to search at.
    /// @param foundNum Pointer to write the found next entrance's ID to.
    /// @return Whether a next entrance was found at the given coordinates.
    bool searchNextNum(u8 fileIdx, float x, float y, int *foundNum);

    /// @brief Move to a new entrance in the same file.
    /// @param destNextGoto The new entrance to go to.
    /// @param faderType The type of fader to use for the scene change.
    void simpleChangeScene(u8 destNextGoto, dFader_c::fader_type_e faderType);

    /// @brief Move to a new entrance in any file.
    /// @param destFile The file to go to.
    /// @param destNextGoto The entrance in that file to go to.
    /// @param faderType The type of fader to use for the scene change.
    void simpleChangeScene(u8 destFile, u8 destNextGoto, dFader_c::fader_type_e faderType);

    /// @brief Go to the next scene using the data in #mNextGotoData.
    void changeScene();

    static void createInstance(EGG::Heap *heap); ///< Creates a class instance on the given heap.
    static void deleteInstance(); ///< Deletes the class instance.

    sNextGotoData mNextGotoData; ///< The data for the entrance the player activated.
    SceneChangeType_e mSceneChangeType; ///< The type of scene change to do when going to the next entrance.
    bool mNextDataSet; ///< Whether the @ref mNextGotoData "entrance data" for the next scene change has been set.
    bool mStartSceneChange; ///< Whether a scene change should be performed next time execute() is called.
    bool mSceneChangeDone; ///< Whether the scene change has been performed.
    u16 mMultiplayerDelay; ///< Number of frames to delay the scene change in multiplayer mode.
    dFader_c::fader_type_e mFaderType; ///< The type of fader to use for the scene change.

    static dNext_c *m_instance; ///< The singleton instance of this class.

    // [Unofficial constants]
    static const int smc_MULTIPLAYER_SCENE_CHANGE_DELAY = 180; ///< The number of frames to delay scene changes in multiplayer mode.
};
