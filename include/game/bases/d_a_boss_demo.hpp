#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_enemy_boss.hpp>
#include <game/sLib/s_GlobalData.hpp>

/// @brief Base class for boss battle cutscene managers.
/// @statetable
/// @ingroup bases
class daBossDemo_c : public dActorState_c {
public:
    struct GlobalData_t {
        bool mForceOtehonClear;
    };

    daBossDemo_c() : dActorState_c() {}
    ~daBossDemo_c() {}

    virtual int create() override;
    virtual int doDelete() override;
    virtual int execute() override;
    virtual int draw() override;
    virtual void deleteReady() override;

    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, Ready); ///< Ready for the boss battle cutscene to start.
    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, BattleStDemo); ///< In the boss battle starting cutscene.
    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, BattleIn); ///< In the boss battle.
    STATE_VIRTUAL_FUNC_DECLARE(daBossDemo_c, BattleEdDemo); ///< In the boss battle ending cutscene.

    /// @brief Returns to the world map and clears the course after finishing it with Super Guide.
    virtual bool abandonRetryAfterOtehonClear();

    /// @brief Returns to the world map without clearing the course after finishing it with Super Guide.
    virtual void retryAfterOtehonClear();

    virtual void startBGM() {} ///< Starts the boss battle background music.
    virtual void stopBGM() {} ///< Stops the boss battle background music.

    virtual fBaseID_e getBossID() { return mBossID; }
    virtual void setBossID(fBaseID_e bossID) { mBossID = bossID; }

    virtual BOOL checkBattleStDemo() { return false; } ///< Returns whether the boss battle start cutscene is ready to play.
    virtual BOOL checkBattleEdDemo() { return mBattleEnd; } ///< Returns whether the boss battle end cutscene is ready to play.
    virtual bool demoScroll() { return true; } ///< Returns whether the camera has finished scrolling to the boss.
    virtual dEnBoss_c *bossSearch() { return nullptr; } ///< Returns the corresponding boss actor.
    virtual void initialize() {} ///< Subclass-specific initialization code.

    void setBattleResult(); ///< Saves the star coins collected and the remaining lives of each player.
    void setMultiCourseClear(); ///< Marks the course as cleared.
    bool checkStartRetry(bool b); ///< Returns whether to show the "Want to try without Super Guide?" prompt.

    bool isOtehonClear(); ///< Returns whether this course was only cleared by the Super Guide. @unofficial

    u32 m_3d0;
    BOOL mBattleEnd; ///< Whether the boss battle has ended.
    fBaseID_e mBossID; ///< The ID of the boss actor.
};
