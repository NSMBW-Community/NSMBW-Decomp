#pragma once
#include <game/bases/d_2d.hpp>
#include <game/bases/d_lyttextBox.hpp>
#include <game/bases/d_tag_processor.hpp>
#include <game/mLib/m_2d.hpp>
#include <nw4r/lyt.h>

/// @brief A 2D layout.
class LytBase_c : public d2d::Multi_c {
public:
    LytBase_c(); ///< Constructs a new layout.
    virtual ~LytBase_c(); ///< Destroys the layout.
    virtual bool build(const char *name, d2d::ResAccMult_c *resAcc);

    /// @brief Finds a text box by name.
    LytTextBox_c *findTextBox(const char *name);

    /// @brief Recursively allocates string buffers for all text boxes in the layout.
    void allocStringBuffer(nw4r::lyt::Pane *pane);

    /// @brief Reads an .arc file which contains the layout.
    /// @param name The name of the archive.
    /// @param allocDirection The allocation direction.
    /// @param isLocalized Whether the layout is in a localized folder (`/<Region>/<Language>/`).
    bool ReadResourceEx(const char *name, int allocDirection, bool isLocalized);

    /// @brief Reads an .arc file which contains the layout.
    /// @param name The name of the archive.
    /// @param isLocalized Whether the layout is in a localized folder (`/<Region>/<Language>/`).
    bool ReadResource(const char *name, bool isLocalized);

    /// @brief Reads an .arc file which contains the layout.
    /// @note Use this function for layouts placed in `/<Region>/Layout/`.
    /// @param name The name of the archive.
    /// @param allocDirection The allocation direction.
    bool ReadResource2(const char *name, int allocDirection);

    /// @brief Reads an .arc file which contains the layout.
    /// @note Use this function for layouts localized in Dutch (`/EU/NedEU/Layout/`).
    /// @decompnote{Not in Shield version.}
    /// @unofficial
    /// @param name The name of the archive.
    /// @param allocDirection The allocation direction.
    bool ReadResource3(const char *name, int allocDirection);

    /// @brief Finds null panes by name and registers them to the list.
    /// @param paneNames The names of the panes to find.
    /// @param panes The list to register the panes to.
    /// @param count The number of panes in paneNames.
    void NPaneRegister(const char **paneNames, nw4r::lyt::Pane **panes, int count);

    /// @brief Finds windows by name and registers them to the list.
    /// @param windowPaneNames The names of the windows to find.
    /// @param panes The list to register the windows to.
    /// @param count The number of windows in windowPaneNames.
    void WPaneRegister(const char **windowPaneNames, nw4r::lyt::Window **panes, int count);

    /// @brief Finds picture panes by name and registers them to the list.
    /// @param picPaneNames The names of the picture panes to find.
    /// @param panes The list to register the picture panes to.
    /// @param count The number of picture panes in picPaneNames.
    void PPaneRegister(const char **picPaneNames, nw4r::lyt::Picture **panes, int count);

    /// @brief Finds text boxes by name and registers them to the list.
    /// @param textboxNames The names of the text boxes to find.
    /// @param panes The list to register the text boxes to.
    /// @param count The number of text boxes in textboxNames.
    void TPaneRegister(const char **textboxNames, LytTextBox_c **panes, int count);

    /// @brief Finds text boxes by name, sets their messages, and registers them to the list.
    /// @param textboxNames The names of the text boxes to find.
    /// @param messageIDs The message IDs of the messages to set for the text boxes.
    /// @param messageGroup The message group of the messages to set for the text boxes.
    /// @param count The number of text boxes in textboxNames.
    void TPaneNameRegister(const char **textboxNames, const int *messageIDs, int messageGroup, int count);

    /// @brief Loads animation resources given by their names.
    /// @param animeNames The names of the animation resources to load.
    /// @param count The number of animations in animeNames.
    void AnimeResRegister(const char **animeNames, int count);

    /// @brief Registers animation groups to the layout.
    /// @param groupNames The names of the animation groups to register.
    /// @param animeIdxs The indices of the animations to use for the animation groups.
    /// @param count The number of animation groups in groupNames.
    void GroupRegister(const char **groupNames, const int *animeIdxs, int count);

    /// @brief Base setup for starting an animation.
    /// @param animeIdx The index of the animation to start.
    void AnimeStartBaseSetup(int animeIdx);

    /// @brief Prepares an animation to be played.
    /// @param animeIdx The index of the animation to start.
    /// @param startAtEnd Whether to skip to the end of the animation.
    void AnimeStartSetup(int animeIdx, bool startAtEnd);

    /// @brief Prepares a looping animation to be played.
    /// @param animeIdx The index of the animation to start.
    void LoopAnimeStartSetup(int animeIdx);

    /// @brief Prepares a reverse animation to be played.
    /// @param animeIdx The index of the animation to start.
    /// @param startAtEnd Whether to skip to the end of the animation.
    void ReverseAnimeStartSetup(int animeIdx, bool startAtEnd);

    /// @brief Prepares an animation to be stopped.
    /// @param animeIdx The index of the animation to be stopped.
    void AnimeEndSetup(int animeIdx);

    /// @brief Prepares all animations to be stopped.
    void AllAnimeEndSetup();

    /// @brief Starts playing all enabled animations.
    void AnimePlay();

    /// @brief Returns whether the specified animation is currently playing.
    /// @param animeIdx The index of the animation to check.
    /// @note If animeIdx is negative, returns whether the most recently started animation is playing.
    bool isAnime(int animeIdx);

    bool isAllAnime(); ///< Returns whether any animation is currently playing.

    void SetScissorMask(const nw4r::lyt::Pane *pane, d2d::ScissorMask &scissorMask); ///< @unofficial

    /// @brief Cleans up all resources used by the layout.
    bool doDelete();

public:
    m2d::AnmGroup_c &getAnmGroup(int index) const { return mpAnimGroup[index]; }

private:
    d2d::ResAccMultLoader_c mResAccessorLoader; ///< The resource loader used to load the layout's resources.

    m2d::AnmResV2_c *mpAnimRes; ///< The animations used by the layout.
    m2d::AnmGroup_c *mpAnimGroup; ///< The animation groups used by the layout.

    bool *mpEnabledAnims; ///< A list of the enabled animations.
    int mAnimCount; ///< The number of animations used by the layout.
    int mAnimGroupCount; ///< The number of animation groups used by the layout.

    int mLastStartedAnimNum; ///< The number of the most recently started animation.

public:
    static TagProcessor_c s_TagPrc; ///< The tag processor for layouts.
};
