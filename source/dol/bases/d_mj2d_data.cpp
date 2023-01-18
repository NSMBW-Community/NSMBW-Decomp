#include <types.h>
#include <constants/game_constants.h>
#include <lib/MSL_C/string.h>
#include <dol/bases/d_mj2d_data.hpp>

const u32 dMj2dGame_c::sDefaultCharacters[PLAYER_COUNT] = {MARIO, LUIGI, YELLOW_TOAD, BLUE_TOAD};
char dMj2dHeader_c::sSaveMagic[4] = {'S', 'M', 'N', 'P'};

dMj2dGame_c::dMj2dGame_c() {
}

void dMj2dGame_c::initialize() {

    // Clear slot and set version and completion
    memset(this, 0, sizeof(dMj2dGame_c));
    mVersion[0] = SAVE_VERSION;
    mVersion[1] = SAVE_SUB_VERSION;
    mGameCompletion |= SAVE_EMPTY;

    // Unlock the rescue stage for each world
    for (int world = 0; world <= NORMAL_WORLD_COUNT; world++) {
        onCourseDataFlag(world, STAGE_RESCUE, GOAL_MASK);
    }

    // Initialize the player data
    for (int player = 0; player < PLAYER_COUNT; player++) {
        setPlrID(player, sDefaultCharacters[player]);
        setPlrMode(player, NO_POWERUP);
        setRest(player, STARTING_LIVES_COUNT);
        setCreateItem(player, NO_CREATE_ITEM);
    }

    // Initialize map data
    for (int world = 0; world < WORLD_COUNT; world++) {
        setKinopioCourseNo(world, STAGE_COUNT);

        for (int enemy = 0; enemy < AMBUSH_ENEMY_COUNT; enemy++) {
            setEnemyCurrNode(world, enemy, -1);
            setEnemyLastDirection(world, enemy, 2);
        }
    }

    setIbaraNow(2);
};

void dMj2dGame_c::versionUpdate() {
    if (mVersion[0] != SAVE_VERSION) {
        initialize();
    }

    mVersion[0] = SAVE_VERSION;
}

void dMj2dGame_c::setPlrID(int player, int character) {
    mPlayerCharacter[player] = character;
}

int dMj2dGame_c::getPlrID(int player) const {
    return mPlayerCharacter[player];
}

void dMj2dGame_c::setPlrMode(int player, u8 powerup) {
    mPlayerPowerup[player] = powerup;
}

int dMj2dGame_c::getPlrMode(int player) const {
    return mPlayerPowerup[player];
}

void dMj2dGame_c::setRest(int player, u8 lives) {
    mPlayerLife[player] = lives;
}

int dMj2dGame_c::getRest(int player) const {
    return mPlayerLife[player];
}

void dMj2dGame_c::setCreateItem(int player, u8 flag) {
    mPlayerCreateItem[player] = flag & STAR_POWER;
}

int dMj2dGame_c::getCreateItem(int player) const {
    return mPlayerCreateItem[player] & STAR_POWER;
}

void dMj2dGame_c::setCoin(int player, s8 coins) {
    mPlayerCoin[player] = coins;
}

s8 dMj2dGame_c::getCoin(int player) const {
    return mPlayerCoin[player];
}

void dMj2dGame_c::setScore(ulong score) {
    mScore = score;
}

int dMj2dGame_c::getScore() const {
    return mScore;
}

void dMj2dGame_c::setStaffCreditHighScore(u16 score) {
    if (mStaffRollHighScore < score) {
        mStaffRollHighScore = score;
    }
}

int dMj2dGame_c::getStaffCreditHighScore() const {
    return mStaffRollHighScore;
}

void dMj2dGame_c::onOtehonMenuOpenFlag(int movie) {
    mOtehonMenuOpen[movie] = true;
}

bool dMj2dGame_c::isOtehonMenuOpenFlag(int movie) const {
    return mOtehonMenuOpen[movie] != 0;
}

void dMj2dGame_c::setCollectCoin(int world, int level, u8 coins) {
    onCourseDataFlag(world, level, coins & COIN_MASK);
}

int dMj2dGame_c::getTotalWorldCollectCoin(int world) {
    int coinCount = 0;
    for (int level = 0; level < STAGE_COUNT; level++) {
        for (u32 coin = 0; coin < STAR_COIN_COUNT; coin++) {
            if (isCollectCoin(world, level, coin) & 0xFF) {
                coinCount++;
            }
        }
    }

    return coinCount;
}

u8 dMj2dGame_c::isCollectCoin(int world, int level, int coin) const {
    return 1 << coin & mStageCompletion[world][level];
}

void dMj2dGame_c::setStartKinokoKind(int world, u8 type) {
    mStartKinokoType[world] = type;
}

int dMj2dGame_c::getStartKinokoKind(int world) const {
    return mStartKinokoType[world];
}

void dMj2dGame_c::setDeathCount(int world, int level, bool isSwitchPressed, u8 count) {
    if (count >= SUPER_GUIDE_DEATH_COUNT && world < NORMAL_WORLD_COUNT && (level <= STAGE_CASTLE || level == STAGE_DOOMSHIP)) {
        mGameCompletion |= SUPER_GUIDE_TRIGGERED;
    }

    // [Hardcoded check for World 3-4]
    if (isSwitchPressed && world == WORLD_3 && level == STAGE_4) {
        setSwitchDeathCount(count);
    } else {
        mDeathCount[world][level] = count;    
    }
}

int dMj2dGame_c::getDeathCount(int world, int level, bool isSwitchPressed) const {
    // [Hardcoded check for World 3-4]
    if (isSwitchPressed && world == WORLD_3 && level == STAGE_4) {
        return getSwitchDeathCount();
    }

    return mDeathCount[world][level];
}

void dMj2dGame_c::setSwitchDeathCount(u8 count) {
    mDeathCountSwitch = count;
}

int dMj2dGame_c::getSwitchDeathCount() const {
    return mDeathCountSwitch;
}

void dMj2dGame_c::setContinue(int player, s8 continues) {
    mPlayerContinue[player] = continues;
}

s8 dMj2dGame_c::getContinue(int player) const {
    return mPlayerContinue[player];
}
void dMj2dGame_c::setStockItem(int item, s8 count) {
    if (count > MAX_STOCK_ITEM) {
        count = MAX_STOCK_ITEM;
    }

    mStockItemCount[item] = count;
}

u8 dMj2dGame_c::isWorldDataFlag(int world, u8 flag) const {
    return flag & mWorldCompletion[world];
}

void dMj2dGame_c::onWorldDataFlag(int world, u8 flag) {
    mWorldCompletion[world] |= flag;
}

void dMj2dGame_c::offWorldDataFlag(int world, u8 flag) {
    mWorldCompletion[world] &= ~flag;
}

int dMj2dGame_c::getCourseDataFlag(int world, int level) const {
    return mStageCompletion[world][level];
}

bool dMj2dGame_c::isCourseDataFlag(int world, int level, ulong flag) const {
    return (mStageCompletion[world][level] & flag) != 0;
}

void dMj2dGame_c::onCourseDataFlag(int world, int level, ulong flag) {
    mStageCompletion[world][level] |= flag;
}

void dMj2dGame_c::offCourseDataFlag(int world, int level, ulong flag) {
    mStageCompletion[world][level] &= ~flag;
}

void dMj2dGame_c::setEnemyRevivalCount(int world, int enemy, u8 count) {
    mEnemyRevivalCount[world][enemy] = count;
}

u8 dMj2dGame_c::getEnemyRevivalCount(int world, int enemy) const {
    return mEnemyRevivalCount[world][enemy];
}

void dMj2dGame_c::setEnemySubWorld(int world, int enemy, u8 subworld) {
    mEnemySubWorld[world][enemy] = subworld;
}

u8 dMj2dGame_c::getEnemySubWorld(int world, int enemy) const {
    return mEnemySubWorld[world][enemy];
}

void dMj2dGame_c::setEnemyCurrNode(int world, int enemy, u8 node) {
    mEnemyPathNode[world][enemy] = node;
}

u8 dMj2dGame_c::getEnemyCurrNode(int world, int enemy) const {
    return mEnemyPathNode[world][enemy];
}

void dMj2dGame_c::setEnemyLastDirection(int world, int enemy, u8 direction) {
    mEnemyDirection[world][enemy] = direction;
}

u8 dMj2dGame_c::getEnemyLastDirection(int world, int enemy) const {
    return mEnemyDirection[world][enemy];
}

void dMj2dGame_c::setKinopioCourseNo(int world, int level) {
    mKinopioCourseNo[world] = level;
}

u8 dMj2dGame_c::getKinopioCourseNo(int world) const {
    return mKinopioCourseNo[world];
}

void dMj2dGame_c::setIbaraNow(int ibaraNow) {
    mIbaraNow = ibaraNow;
}

u8 dMj2dGame_c::getIbaraNow() const {
    return mIbaraNow;
}

dMj2dHeader_c::dMj2dHeader_c() {
}

void dMj2dHeader_c::initialize() {
    memset(this, 0, sizeof(dMj2dHeader_c));

    mMagic[0] = sSaveMagic[0];
    mMagic[1] = sSaveMagic[1];
    mMagic[2] = sSaveMagic[2];
    mMagic[3] = sSaveMagic[3];

    mVersion[0] = SAVE_VERSION;
    mVersion[1] = SAVE_SUB_VERSION;
}

void dMj2dHeader_c::setSelectFileNo(s8 fileNum) {
    mLastSelectedFile = fileNum;
}

u16 dMj2dHeader_c::getFreeModePlayCount(int world, int level) const {
    return mFreeModePlayCount[world][level];
}

void dMj2dHeader_c::setFreeModePlayCount(int world, int level, u16 count) {
    mFreeModePlayCount[world][level] = count;
}

u16 dMj2dHeader_c::getCoinBattlePlayCount(int world, int level) const {
    return mCoinBattlePlayCount[world][level];
}

void dMj2dHeader_c::setCoinBattlePlayCount(int world, int level, u16 count) {
    mCoinBattlePlayCount[world][level] = count;
}

void dMj2dHeader_c::setMultiModeCompletion(int world) {
    mExtraModesUnlockedWorlds |= 1 << world;
}

bool dMj2dHeader_c::getMultiModeCompletion(int world) const {
    return mExtraModesUnlockedWorlds & (1 << world);
}

void dMj2dHeader_c::versionUpdate() {
    if (mVersion[0] != SAVE_VERSION) {
        initialize();
    }

    mVersion[0] = SAVE_VERSION;
}
