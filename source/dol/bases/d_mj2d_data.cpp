#include <types.h>
#include <lib/MSL_C/string.h>
#include <game/bases/d_mj2d_data.hpp>

const u32 dMj2dGame_c::sDefaultCharacters[PLAYER_COUNT] = {PLAYER_MARIO, PLAYER_LUIGI, PLAYER_YELLOW_TOAD, PLAYER_BLUE_TOAD};

dMj2dGame_c::dMj2dGame_c() {}

void dMj2dGame_c::initialize() {

    // Clear slot and set version and completion
    memset(this, 0, sizeof(dMj2dGame_c));
    mRevision[0] = SAVE_REVISION_MAJOR;
    mRevision[1] = SAVE_REVISION_MINOR;
    mGameCompletion |= SAVE_EMPTY;

    // Unlock the rescue stage for each world
    for (int world = 0; world <= NORMAL_WORLD_COUNT; world++) {
        onCourseDataFlag(world, STAGE_RESCUE, GOAL_MASK);
    }

    // Initialize the player data
    for (int player = 0; player < PLAYER_COUNT; player++) {
        setPlrID(player, sDefaultCharacters[player]);
        setPlrMode(player, POWERUP_NONE);
        setRest(player, STARTING_LIVES_COUNT);
        setCreateItem(player, CREATE_ITEM_NONE);
    }

    // Initialize map data
    for (int world = 0; world < WORLD_COUNT; world++) {
        setKinopioCourseNo(world, STAGE_COUNT);

        for (int enemy = 0; enemy < AMBUSH_ENEMY_COUNT; enemy++) {
            setCSEnemyPosIndex(world, enemy, -1);
            setCSEnemyWalkDir(world, enemy, 2);
        }
    }

    setIbaraNow(2);
};

void dMj2dGame_c::versionUpdate() {
    if (mRevision[0] != SAVE_REVISION_MAJOR) {
        initialize();
    }

    mRevision[0] = SAVE_REVISION_MAJOR;
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
    mPlayerCreateItem[player] = flag & CREATE_ITEM_STAR_POWER;
}

int dMj2dGame_c::getCreateItem(int player) const {
    return mPlayerCreateItem[player] & CREATE_ITEM_STAR_POWER;
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

int dMj2dGame_c::getStaffCreditHighScore() {
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

void dMj2dGame_c::setCSEnemyRevivalCnt(int world, int enemy, int count) {
    mEnemyRevivalCount[world][enemy] = count;
}

u8 dMj2dGame_c::getCSEnemyRevivalCnt(int world, int enemy) const {
    return mEnemyRevivalCount[world][enemy];
}

void dMj2dGame_c::setCSEnemySceneNo(int world, int enemy, u8 subworld) {
    mEnemySceneNo[world][enemy] = subworld;
}

u8 dMj2dGame_c::getCSEnemySceneNo(int world, int enemy) const {
    return mEnemySceneNo[world][enemy];
}

void dMj2dGame_c::setCSEnemyPosIndex(int world, int enemy, u8 node) {
    mEnemyPosIndex[world][enemy] = node;
}

u8 dMj2dGame_c::getCSEnemyPosIndex(int world, int enemy) const {
    return mEnemyPosIndex[world][enemy];
}

void dMj2dGame_c::setCSEnemyWalkDir(int world, int enemy, u8 direction) {
    mEnemyWalkDir[world][enemy] = direction;
}

u8 dMj2dGame_c::getCSEnemyWalkDir(int world, int enemy) const {
    return mEnemyWalkDir[world][enemy];
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

    mMagic[0] = SAVE_MAGIC[0];
    mMagic[1] = SAVE_MAGIC[1];
    mMagic[2] = SAVE_MAGIC[2];
    mMagic[3] = SAVE_MAGIC[3];

    mRevision[0] = SAVE_REVISION_MAJOR;
    mRevision[1] = SAVE_REVISION_MINOR;
}

void dMj2dHeader_c::setSelectFileNo(s8 fileNum) {
    mLastSelectedFile = fileNum;
}

u16 dMj2dHeader_c::getPlayCountFreeMode(int world, int level) const {
    return mPlayCountFreeMode[world][level];
}

void dMj2dHeader_c::setPlayCountFreeMode(int world, int level, int count) {
    mPlayCountFreeMode[world][level] = count;
}

u16 dMj2dHeader_c::getPlayCountCoinBattle(int world, int level) const {
    return mPlayCountCoinBattle[world][level];
}

void dMj2dHeader_c::setPlayCountCoinBattle(int world, int level, int count) {
    mPlayCountCoinBattle[world][level] = count;
}

void dMj2dHeader_c::onMultiWorldOpenFlag(int world) {
    mMultiWorldOpenFlag |= 1 << world;
}

bool dMj2dHeader_c::isMultiWorldOpenFlag(int world) {
    return mMultiWorldOpenFlag & (1 << world);
}

void dMj2dHeader_c::versionUpdate() {
    if (mRevision[0] != SAVE_REVISION_MAJOR) {
        initialize();
    }

    mRevision[0] = SAVE_REVISION_MAJOR;
}
