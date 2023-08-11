#pragma once
#include <types.h>
#include <constants/game_constants.h>
/// @file

#define SAVE_VERSION 14 ///< The current save version.
#define SAVE_SUB_VERSION 0 ///< The current save subversion.

class dSaveMng_c;

/// @brief Save data header holder.
/// @details The data stored here is temporary, as it is discarded unless the game is saved.
/// Size must be 32-byte aligned.
class dMj2dHeader_c {
    public:
        dMj2dHeader_c(); ///< Constructs the holder.

        void initialize(); ///< Initializes the header data.

        /// @brief Gets the Free Mode play count for the given world/level. See ::mFreeModePlayCount.
        /// @unofficial
        u16 getFreeModePlayCount(int world, int level) const;

        /// @brief Sets the Free Mode play count for the given world/level. See ::mFreeModePlayCount.
        /// @unofficial
        void setFreeModePlayCount(int world, int level, u16 count);

        /// @brief Gets the Coin Battle play count for the given world/level. See ::mCoinBattlePlayCount.
        /// @unofficial
        u16 getCoinBattlePlayCount(int world, int level) const;

        /// @brief Sets the Coin Battle play count for the given world/level. See ::mCoinBattlePlayCount.
        /// @unofficial
        void setCoinBattlePlayCount(int world, int level, u16 count);

        /// @brief Unlocks the given world in extra modes.
        /// @unofficial
        void setMultiModeCompletion(int world);

        /// @brief Checks if the given world is unlocked in extra modes.
        /// @unofficial
        bool getMultiModeCompletion(int world) const;

private:
        /// @brief Checks that the save data version matches the current one and clears the slot if not.
        void versionUpdate();
        void setSelectFileNo(s8 file); ///< Sets the last used save data slot.

        char mMagic[4]; ///< The savegame magic.
        u8 mVersion[2]; ///< The save data version and subversion. See ::SAVE_VERSION and ::SAVE_SUB_VERSION.
        u8 mLastSelectedFile; ///< The last selected save data slot.
        u8 mUnknown7; ///< @unused [Most likely declared as padding].

        /// @brief The play count of each level in Free Mode.
        /// @details [Value is increased by 0x100 for every playthrough until 10000].
        u16 mFreeModePlayCount[WORLD_COUNT][STAGE_COUNT];

        /// @brief The play count of each level in Coin Battle.
        /// @details [Value is increased by 0x100 for every playthrough until 10000].
        u16 mCoinBattlePlayCount[WORLD_COUNT][STAGE_COUNT];

        /// @brief The worlds unlocked in Extra Modes.
        /// @details Playing a world in extra modes requires said world to be unlocked in at least
        /// one save slot. Value is a bitfield.
        u16 mExtraModesUnlockedWorlds;
        u16 mUnknown69A; ///< @unused [Most likely declared as padding].

        u32 mChecksum; ///< The CRC32 checksum of the above data (excluding ::mMagic).

        /// @brief The expected savegame magic.
        /// @details Value is @p SMNP for PAL games.
        /// @unofficial Is this even part of the class?
        static char sSaveMagic[4];

        friend class dSaveMng_c;
};

/// @brief Save slot data holder.
/// @details The data stored here is temporary, as it is discarded unless the game is saved.
/// Size must be 32-byte aligned.
class dMj2dGame_c {
public:

    /// @brief The completion status of each level.
    enum COURSE_COMPLETION_e {
        COIN1_COLLECTED = 0x1, ///< The first Star Coin was obtained.
        COIN2_COLLECTED = 0x2, ///< The second Star Coin was obtained.
        COIN3_COLLECTED = 0x4, ///< The third Star Coin was obtained.
        COIN_MASK = COIN1_COLLECTED | COIN2_COLLECTED | COIN3_COLLECTED,
        GOAL_NORMAL = 0x10, ///< The regular exit was cleared.
        GOAL_SECRET = 0x20, ///< The secret exit was cleared.
        GOAL_MASK = GOAL_NORMAL | GOAL_SECRET,
        SUPER_GUIDE_GOAL_NORMAL = 0x80, ///< The regular exit was cleared with the Super Guide.
        SUPER_GUIDE_GOAL_SECRET = 0x100, ///< The secret exit was cleared with the Super Guide.
        SUPER_GUIDE_GOAL_MASK = SUPER_GUIDE_GOAL_NORMAL | SUPER_GUIDE_GOAL_SECRET
    };

    /// @brief The completion status of each world.
    enum WORLD_COMPLETION_e {
        WORLD_UNLOCKED = 0x1
    };

    /// @brief The overall completion status of the save slot.
    enum GAME_COMPLETION_e {
        SAVE_EMPTY = 0x1, ///< The save file is empty.
        FINAL_BOSS_BEATEN = 0x2, ///< The final boss was beaten.
        GOAL_ALL = 0x4, ///< All levels were completed.
        COIN_ALL = 0x8, ///< All the Star Coins in the regular worlds were obtained.
        COIN_ALL_SPECIAL = 0x10, ///< All the Star Coins in the special worlds were obtained.
        GAME_COMPLETED = 0x20, ///< All the above objectives were completed.
        SUPER_GUIDE_TRIGGERED = 0x40 ///< The player died at least ::SUPER_GUIDE_DEATH_COUNT times in a single stage.
    };

    /// @brief The Toad House types at the start node of each world.
    /// @details Values marked with R are used for houses that spawn after a successful Toad Rescue mission.
    enum START_KINOKO_KIND_e {
        TOAD_HOUSE_NONE,
        TOAD_HOUSE_YELLOW,
        TOAD_HOUSE_RED,
        TOAD_HOUSE_GREEN,
        TOAD_HOUSE_YELLOW_R,
        TOAD_HOUSE_RED_R,
        TOAD_HOUSE_GREEN_R
    };

    /// @brief Constructs the holder.
    dMj2dGame_c();

    /// @brief Deletes the holder.
    ~dMj2dGame_c() {};

    void initialize(); ///< Initializes the slot data.

    void setPlrID(int player, int character); ///< Sets @p player 's character. See ::PLAYER_CHARACTER_e.
    int getPlrID(int player) const; ///< Gets @p player 's character. See ::PLAYER_CHARACTER_e.

    void setPlrMode(int player, u8 powerup); ///< Sets @p player 's powerup. See ::PLAYER_POWERUP_e.
    int getPlrMode(int player) const; ///< Gets @p player 's powerup. See ::PLAYER_POWERUP_e.

    void setRest(int player, u8 lives); ///< Sets @p player 's life count.
    int getRest(int player) const; ///< Gets @p player 's life count.

    /// @brief Sets @p player 's createItem flag. See ::PLAYER_CREATE_ITEM_e.
    /// @details Flags other than ::CREATE_ITEM_STAR_POWER are ignored.
    void setCreateItem(int player, u8 flag);

    /// @brief Gets @p player 's createItem flag. See ::PLAYER_CREATE_ITEM_e.
    /// @details Flags other than ::CREATE_ITEM_STAR_POWER are ignored.
    int getCreateItem(int player) const;

    void setCoin(int player, s8 coins); ///< Sets @p player 's coin count.
    s8 getCoin(int player) const; ///< Gets @p player 's coin count.

    void setScore(ulong score); ///< Sets the score.
    int getScore() const; ///< Gets the score.

    void setStaffCreditHighScore(u16 score); ///< Sets the staff credit high score.
    int getStaffCreditHighScore() const; ///< Gets the staff credit high score.

    /// @brief Sets the given hint movie as bought.
    void onOtehonMenuOpenFlag(int movie);

    /// @brief Checks if the given hint movie was bought.
    bool isOtehonMenuOpenFlag(int movie) const;

    /// @brief Sets the obtained Star Coin(s) for the given world/level. See ::COURSE_COMPLETION_e.
    /// @details Other flags are discarded.
    void setCollectCoin(int world, int level, u8 coins);

    /// @brief Gets the obtainable Star Coin count for the given world.
    int getTotalWorldCollectCoin(int world);

    /// @brief Checks if the given Star Coin has been obtained in the given world/level.
    u8 isCollectCoin(int world, int level, int coin) const;

    /// @brief Sets the starting Toad House type in the given world. See ::START_KINOKO_KIND_e.
    void setStartKinokoKind(int world, u8 type);

    /// @brief Gets the starting Toad House type in the given world. See ::START_KINOKO_KIND_e.
    int getStartKinokoKind(int world) const;

    /**
     * @brief Sets the death count in a specific stage.
     * @param world The world.
     * @param level The level.
     * @param isSwitchPressed If the worldmap switch was activated [only used for level 3-4].
     * @param count The death count.
     */
    void setDeathCount(int world, int level, bool isSwitchPressed, u8 count);

    /**
     * @brief Gets the death count in a specific stage.
     * @param world The world.
     * @param level The level.
     * @param isSwitchPressed If the worldmap switch was activated [only used for level 3-4].
     * @return The death count.
     */
    int getDeathCount(int world, int level, bool isSwitchPressed) const;

    void setContinue(int player, s8 continues); ///< Sets @p player 's continue count.
    s8 getContinue(int player) const; ///< Gets @p player 's continue count.

    void setStockItem(int item, s8 count); ///< Sets the inventory amount for the given item.
    inline s8 getStockItem(int item) { return mStockItemCount[item]; }; // [why was this inlined?]

    /// @brief Checks if the completion flag(s) for the given world is set. See ::WORLD_COMPLETION_e.
    u8 isWorldDataFlag(int world, u8 flag) const;

    /// @brief Sets the specified flag(s) for the given world. See ::WORLD_COMPLETION_e.
    void onWorldDataFlag(int world, u8 flag);

    /// @brief Unsets the specified flag(s) for the given world. See ::WORLD_COMPLETION_e.
    void offWorldDataFlag(int world, u8 flag);

    /// @brief Checks if the completion flag for the given world/level is set. See ::COURSE_COMPLETION_e.
    bool isCourseDataFlag(int world, int level, ulong flag) const;

    /// @brief Sets the completion flag(s) for the given world/level. See ::COURSE_COMPLETION_e.
    void onCourseDataFlag(int world, int level, ulong flag);

    /// @brief Unsets the completion flag(s) for the given world/level. See ::COURSE_COMPLETION_e.
    void offCourseDataFlag(int world, int level, ulong flag);

    /// @brief Gets all the completion flags for the given world/level. See ::COURSE_COMPLETION_e.
    int getCourseDataFlag(int world, int level) const;

    /// @brief Sets the revival counter for the given map enemy.
    void setCSEnemyRevivalCnt(int world, int enemy, int count);

    /// @brief Gets the revival counter for the given map enemy.
    u8 getCSEnemyRevivalCnt(int world, int enemy) const;

    /// @brief Sets the subworld number for the given map enemy.
    void setCSEnemySceneNo(int world, int enemy, u8 subworld);

    /// @brief Gets the subworld number for the given map enemy.
    u8 getCSEnemySceneNo(int world, int enemy) const;

    /// @brief Sets the path node for the given map enemy.
    void setCSEnemyPosIndex(int world, int enemy, u8 node);

    /// @brief Gets the path node for the given map enemy.
    u8 getCSEnemyPosIndex(int world, int enemy) const;

    /// @brief Sets the path direction for the given map enemy.
    /// @details Values are: 0 (forwards), 1 (backwards) and 2 (initial value).
    /// @todo Turn the value list into an enum and move it to the appropriate section once decompiled.
    void setCSEnemyWalkDir(int world, int enemy, u8 direction);

    /// @brief Gets the path direction for the given map enemy.
    u8 getCSEnemyWalkDir(int world, int enemy) const;

    /// @brief Sets the Toad rescue level for the given world.
    void setKinopioCourseNo(int world, int level);
    u8 getKinopioCourseNo(int world) const; ///< Gets the Toad rescue level for the given world.

    void setIbaraNow(int ibaraNow); ///< Sets the worldmap vine reshuffle counter.
    u8 getIbaraNow() const; ///< Gets the worldmap vine reshuffle counter.

private:
    /// @brief Checks that the save data version matches the current one and clears the slot if not.
    void versionUpdate();

    /// @brief Sets the death count for World 3-4 for the worldmap switch-enabled variant.
    void setSwitchDeathCount(u8 count);

    /// @brief Gets the death count for World 3-4 for the worldmap switch-enabled variant.
    int getSwitchDeathCount() const;

    u8 mVersion[2]; ///< The save data version and subversion. See ::SAVE_VERSION and ::SAVE_SUB_VERSION.

    u8 mGameCompletion; ///< The overall completion of the save slot. See ::GAME_COMPLETION_e.

    u8 mCurrentWorld; ///< The world the player is currently in.
    u8 mCurrentSubWorld; ///< The subworld the player is currently in.
    u8 mCurrentPathNode; ///< The path node the player is currently on. @todo [Verify].

    /// @brief The worldmap vine reshuffle counter.
    /// @details [Value decreases every time a level is played. If it reaches zero, the vines are moved].
    u8 mIbaraNow;

    u8 mSwitchOn; ///< The status of the worldmap switch.
    u8 mUnknown8; ///< @unused

    u8 mStockItemCount[POWERUP_COUNT]; ///< The inventory amount for each item.

    /// @brief The starting Toad House type for each world. See ::START_KINOKO_KIND_e.
    u8 mStartKinokoType[WORLD_COUNT];

    u8 mPlayerContinue[PLAYER_COUNT]; ///< The continue count for each player.
    s8 mPlayerCoin[PLAYER_COUNT]; ///< The coin count for each player.
    u8 mPlayerLife[PLAYER_COUNT]; ///< The life count for each player.
    u8 mPlayerCreateItem[PLAYER_COUNT]; ///< The Star Power flag for each player. See ::PLAYER_CREATE_ITEM_e.
    u8 mPlayerCharacter[PLAYER_COUNT]; ///< The character for each player. See ::PLAYER_CHARACTER_e.
    u8 mPlayerPowerup[PLAYER_COUNT]; ///< The powerup for each player. See ::PLAYER_POWERUP_e.

    u8 mWorldCompletion[WORLD_COUNT]; ///< The completion flags for each world. See ::WORLD_COMPLETION_e.
    u8 mEnemyRevivalCount[WORLD_COUNT][AMBUSH_ENEMY_COUNT]; ///< The revival counter for each map enemy.

    u16 mUnknown64; ///< @unused
    u16 mStaffRollHighScore; ///< The staff credits high score.
    u32 mScore; ///< The regular score.

    /// @brief The completion flags for each level. See ::COURSE_COMPLETION_e.
    u32 mStageCompletion[WORLD_COUNT][STAGE_COUNT];
    bool mOtehonMenuOpen[HINT_MOVIE_COUNT]; ///< The hint movie bought status for each movie.
    u8 mKinopioCourseNo[WORLD_COUNT]; ///< The Toad Rescue level for each world.

    u8 mEnemySceneNo[WORLD_COUNT][AMBUSH_ENEMY_COUNT]; ///< The subworld number for each map enemy.
    u8 mEnemyPosIndex[WORLD_COUNT][AMBUSH_ENEMY_COUNT]; ///< The path node for each map enemy.
    u8 mEnemyWalkDir[WORLD_COUNT][AMBUSH_ENEMY_COUNT]; ///< The movement direction for each map enemy.

    /// @brief The death count for each level.
    /// @details The value is cumulative, and not reset after clearing the level.
    u8 mDeathCount[WORLD_COUNT][STAGE_COUNT];
    u8 mDeathCountSwitch; ///< The death count for the worldmap switch variant of World 3-4. See ::mDeathCount.

    u8 pad[0x13]; // [Aligns the data to 32]
    u32 mChecksum; ///< The CRC32 checksum of the above data.

    /// @brief The default character for each player.
    /// @unofficial Should be part of the class in theory.
    static const u32 sDefaultCharacters[4];

    friend class dSaveMng_c;
};
