#include <game/bases/d_next.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_cd.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_snd_scene_mgr.hpp>
#include <game/mLib/m_heap.hpp>

dNext_c *dNext_c::m_instance;

dNext_c::dNext_c() : mNextDataSet(false), mStartSceneChange(false), mSceneChangeDone(false), mMultiplayerDelay(smc_MULTIPLAYER_SCENE_CHANGE_DELAY) {
    m_instance = this;
}

void dNext_c::createInstance(EGG::Heap *heap) {
    EGG::Heap *prevHeap = mHeap::setCurrentHeap(heap);
    new dNext_c();
    mHeap::setCurrentHeap(prevHeap);
}

void dNext_c::deleteInstance() {
    if (m_instance == nullptr) {
        return;
    }
    dNext_c *instance = m_instance;
    m_instance = nullptr;
    delete instance;
}

void dNext_c::initialize() {
    mNextDataSet = false;
    mStartSceneChange = false;
    mSceneChangeDone = false;
    mMultiplayerDelay = smc_MULTIPLAYER_SCENE_CHANGE_DELAY;
}

void dNext_c::setChangeSceneNextDat(u8 fileIdx, u8 nextGoto, dFader_c::fader_type_e faderType) {
    if (!mNextDataSet) {
        setOwnNextData(fileIdx, nextGoto);
        mNextDataSet = true;
        mSceneChangeDone = false;
        mFaderType = faderType;
    }
}

void dNext_c::setOwnNextData(u8 fileIdx, u8 nextGoto) {
    dCdFile_c *file = dCd_c::m_instance->getFileP(fileIdx);
    mNextGotoData = *file->getNextGotoP(nextGoto);

    u8 destFile = mNextGotoData.mDestFile - 1;
    if (destFile == 255) {
        destFile = fileIdx;
    }
    mNextGotoData.mDestFile = destFile;

    if (mNextGotoData.mDestID == 255) {
        mNextGotoData.mDestID = dCd_c::m_instance->getFileP(fileIdx)->mpOptions->mInitialNextGotoID;
    }
}

bool dNext_c::isNextDifferent(u8 fileIdx, u8 nextGoto) {
    if (mNextDataSet) {
        dCdFile_c *file = dCd_c::m_instance->getFileP(fileIdx);
        sNextGotoData *data = file->getNextGotoP(nextGoto);
        if (data->mID != mNextGotoData.mID) {
            return true;
        }

        u8 destFile = data->mDestFile - 1;
        if (destFile == 255) {
            destFile = fileIdx;
        }
        if (destFile != mNextGotoData.mDestFile) {
            return true;
        }
    }
    return false;
}

bool dNext_c::searchNextNum(u8 fileIdx, float x, float y, int *foundNum) {
    dCdFile_c *file = dCd_c::m_instance->getFileP(fileIdx);

    mVec2_c pos(x, -y);
    sNextGotoData *curr = file->mpNextGotos;
    for (int i = 0; i < file->mNextGotoCount; i++) {
        if (!(curr->mFlags & NEXT_GOTO_FLAG_ONLY_EXIT)) {
            mVec2_c nextGotoSize;
            switch (curr->mType) {
                case NEXT_GOTO_TYPE_14:
                case NEXT_GOTO_TYPE_MINI_PIPE_U:
                case NEXT_GOTO_TYPE_MINI_PIPE_D:
                case NEXT_GOTO_TYPE_MINI_PIPE_L:
                case NEXT_GOTO_TYPE_MINI_PIPE_R:
                    nextGotoSize.set(16.0f, 16.0f);
                    break;
                case NEXT_GOTO_TYPE_15:
                    nextGotoSize.set(48.0f, 16.0f);
                    break;
                case NEXT_GOTO_TYPE_PIPE_L:
                case NEXT_GOTO_TYPE_PIPE_R:
                    nextGotoSize.set(16.0f, 32.0f);
                    break;
                case NEXT_GOTO_TYPE_WATER_TANK:
                    nextGotoSize.set(32.0f, 32.0f);
                    break;
                default:
                    nextGotoSize.set(32.0f, 16.0f);
                    break;
            }

            float gdX = curr->mX;
            float gdY = curr->mY;
            if (
                gdX <= pos.x && pos.x < gdX + nextGotoSize.x &&
                gdY <= pos.y && pos.y < gdY + nextGotoSize.y
            ) {
                s8 id = curr->mID;
                *foundNum = id;
                return true;
            }
        }
        curr++;
    }
    return false;
}

void dNext_c::simpleChangeScene(u8 destNextGoto, dFader_c::fader_type_e faderType) {
    simpleChangeScene(dScStage_c::m_instance->mCurrFile, destNextGoto, faderType);
}

void dNext_c::simpleChangeScene(u8 destFile, u8 destNextGoto, dFader_c::fader_type_e faderType) {
    if (mNextDataSet) {
        return;
    }

    dCdFile_c *file = dCd_c::m_instance->getFileP(destFile);
    mNextGotoData = *file->getNextGotoP(destNextGoto);

    mNextGotoData.mDestFile = destFile;
    mNextGotoData.mDestID = destNextGoto;
    mNextDataSet = true;
    mSceneChangeDone = false;
    mStartSceneChange = true;
    mMultiplayerDelay = 0;
    mFaderType = faderType;
}

void dNext_c::changeScene() {
    dActor_c::mExecStopReq |=
        BIT_FLAG(dActor_c::STAGE_ACTOR_GENERIC) | BIT_FLAG(dActor_c::STAGE_ACTOR_PLAYER) |
        BIT_FLAG(dActor_c::STAGE_ACTOR_YOSHI) | BIT_FLAG(dActor_c::STAGE_ACTOR_ENEMY);

    dScStage_c *stage = dScStage_c::m_instance;
    u8 world = stage->getCurrWorld();

    if (dScStage_c::m_gameMode == dInfo_c::GAME_MODE_HINT_MOVIE) {
        dFader_c::setFader(dFader_c::CIRCLE_MIDDLE);
    } else {
        dFader_c::setFader(mFaderType);
    }

    if (mNextGotoData.mIsLevelExit) {
        dScStage_c::m_exitMode = dScStage_c::EXIT_0;
        dScWMap_c::setNextWorldScene(world, dScWMap_c::m_SceneNo, 0);
        return;
    }

    dCdFile_c *file = dCd_c::m_instance->getFileP(mNextGotoData.mDestFile);
    sNextGotoData *data = file->getNextGotoP(mNextGotoData.mDestID);
    if (stage->mCurrFile != mNextGotoData.mDestFile) {
        mSceneChangeType = NEXT_DIFFERENT_FILE;
    } else if (data->mArea != stage->mCurrArea) {
        mSceneChangeType = NEXT_DIFFERENT_AREA;
    } else {
        mSceneChangeType = NEXT_SAME_AREA;
    }

    u8 bgm = file->getAreaBgm(data->mArea);
    dAudio::setMainBgmNo(bgm);
    u8 mode = file->getAreaBgmMode(data->mArea);
    dAudio::setBgmMode(mode & 0x0F);
    SndSceneMgr::sInstance->FUN_8019d5b0(mode >> 4);

    u32 param = ACTOR_PARAM_GEN(dScStage_c, File, mNextGotoData.mDestFile);
    param |= ACTOR_PARAM_GEN(dScStage_c, NextGotoID, mNextGotoData.mDestID);

    mSceneChangeDone = true;
    dScStage_c::m_exitMode = dScStage_c::EXIT_3;
    dScene_c::setNextScene(fProfile::STAGE, param, false);
}

void dNext_c::execute() {
    if (dScStage_c::m_isCourseOut || !mNextDataSet || !mStartSceneChange || mSceneChangeDone) {
        return;
    }

    if (daPyMng_c::mNum > 1 && mMultiplayerDelay > 0) {
        mMultiplayerDelay--;
        return;
    }

    changeScene();
}
