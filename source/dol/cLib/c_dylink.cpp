#include <cstdio>
#include <game/cLib/c_dylink.hpp>
#include <runtime/rel.h>

DynamicModuleControlBase *DynamicModuleControlBase::mFirst;
DynamicModuleControlBase *DynamicModuleControlBase::mLast;

DynamicModuleControlBase::~DynamicModuleControlBase() {
    if (mpPrev != nullptr) {
        mpPrev->mpNext = mpNext;
    }
    if (mpNext != nullptr) {
        mpNext->mpPrev = mpPrev;
    }
    if (mFirst == this) {
        mFirst = mpNext;
    }
    if (mLast == this) {
        mLast = mpPrev;
    }
    mpNext = nullptr;
    mpPrev = nullptr;
}

DynamicModuleControlBase::DynamicModuleControlBase() : mUsageCount(0), mLinkCount(0), mpNext(nullptr) {
    if (mFirst == nullptr) {
        mFirst = this;
    }
    mpPrev = mLast;
    if (mpPrev != nullptr) {
        mpPrev->mpNext = this;
    }
    mLast = this;
}

bool DynamicModuleControlBase::link() {
    if (mUsageCount == 0) {
        // Only load and link if this module isn't already loaded
        do_load();
        if (!do_link()) {
            return false;
        }
        if (mLinkCount < 0xffff) {
            mLinkCount++;
        }
    }
    if (mUsageCount < 0xffff) {
        mUsageCount++;
    }
    return true;
}

// [Deadstripped, but was presumably here because of the weak virtuals]
bool DynamicModuleControlBase::unlink() {
    mUsageCount--;
    if (mUsageCount == 0) {
        do_unlink();
        mLinkCount--;
        do_unload();
    }
    return true;
}

bool DynamicModuleControlBase::load_async() {
    if (mUsageCount == 0) {
        return do_load_async();
    }
    return true;
}

void DynamicModuleControlBase::dump() {}

size_t DynamicModuleControlBase::getModuleSize() const {
    return 0;
}

const char *DynamicModuleControl::sModulesDir = "/rels";

size_t DynamicModuleControl::sAllocBytes;
EGG::DvdFile *DynamicModuleControl::sDvdFile;
mDvd_toMainRam_c *sDvdCommand;
EGG::ExpHeap *DynamicModuleControl::sDylinkHeap;
size_t DynamicModuleControl::sHeapMinAllocatableSize;
size_t DynamicModuleControl::sHeapMinTotalFreeSize;

DynamicModuleControl::DynamicModuleControl(const char *name, EGG::ExpHeap *heap) : DynamicModuleControlBase(),
    mpModule(nullptr),
    mpBss(nullptr),
    mPrologReturn(0),
    mModuleName(name),
    mType(MODULE_TYPE_UNKNOWN),
    mLoadCount(0),
    mChecksum(0),
    mModuleSize(0),
    mpDvdCallback(nullptr),
    mpHeap(heap),
    mDebugMapFile() {
}

DynamicModuleControl::~DynamicModuleControl() {}

const char *DynamicModuleControl::getModuleName() const {
    return mModuleName;
}

void DynamicModuleControl::initialize(EGG::ExpHeap *heap) {
    sDylinkHeap = heap;

    sDvdFile = (EGG::DvdFile *) heap->alloc(sizeof(EGG::DvdFile), 4);
    new(sDvdFile) EGG::DvdFile();
    sAllocBytes = sizeof(EGG::DvdFile);

    sHeapMinAllocatableSize = heap->getAllocatableSize(4);
    sHeapMinTotalFreeSize = heap->getTotalFreeSize();
}

void *DynamicModuleControl::callback(void *self) {
    DynamicModuleControl *dmc = (DynamicModuleControl *) self;
    return (void *) dmc->do_load();
}

void DynamicModuleControl::checkHeapStatus() {
    if (mpHeap != sDylinkHeap) {
        return;
    }
    size_t allocatableSize = mpHeap->getAllocatableSize(4);
    if (sHeapMinAllocatableSize > allocatableSize) {
        sHeapMinAllocatableSize = allocatableSize;
    }
    size_t totalFreeSize = mpHeap->getTotalFreeSize();
    if (sHeapMinTotalFreeSize > totalFreeSize) {
        sHeapMinTotalFreeSize = totalFreeSize;
    }
}

u16 calcSum2(const u16 *data, size_t count) {
    u16 sum = 0;
    while (count > 0) {
        sum += *data;
        count -= 2;
        data++;
    }
    return sum;
}

bool DynamicModuleControl::do_load() {
    // Check if already loaded
    if (mpModule != nullptr) {
        return true;
    }

    int attempts = 0;
    if (mpHeap == nullptr) {
        mpHeap = sDylinkHeap;
    }

    // On retry, free memory from the previous attempt
retry_load:
    if (mpModule != nullptr) {
        mpHeap->free(mpModule);
        mpModule = nullptr;
    }

    char buf[0x40];
    snprintf(buf, ARRAY_SIZE(buf), "%s/%sNP.rel", sModulesDir, mModuleName);
    if (mpModule == nullptr) {
        sDvdCommand = mDvd_toMainRam_c::create(buf, 1, mpHeap);
        if (sDvdCommand != nullptr) {
            sDvdCommand->waitDone();
            checkHeapStatus();
            OSModuleHeader *module = (OSModuleHeader *) sDvdCommand->mpData;
            sDvdCommand->mpData = nullptr;
            mpModule = module;
            sDvdCommand->destroy();
            sDvdCommand = nullptr;

            if (mpModule != nullptr) {
                mModuleSize = 0;
                mType = MODULE_TYPE_DVD;
            }
        }
    }
    if (mpModule == nullptr) {
        return false;
    }

    // [The following seems to be a leftover debug check to ensure the data was correctly read]
    if (mModuleSize > 0) {
        u16 actualChecksum = calcSum2((u16 *) mpModule, mModuleSize);

        // On first load, load the file again to verify
        if (mLoadCount == 0) {
            mChecksum = actualChecksum;
            mLoadCount++;
            goto retry_load;
        }

        // If the checksum fails, try to read the data 2 more times before quitting
        if (actualChecksum != mChecksum) {
            mLoadCount = 0;
            if (attempts < 2) {
                attempts++;
                goto retry_load;
            }
            return false;
        }
        if (mLoadCount < 255) {
            mLoadCount++;
        }
    } else {
        if (mLoadCount < 255) {
            mLoadCount++;
        }
    }

    checkHeapStatus();
    return true;
}

bool DynamicModuleControl::do_load_async() {
    if (mpDvdCallback == nullptr) {
        if (mpModule != nullptr) {
            return true;
        }
        mpDvdCallback = mDvd_callback_c::create(callback, this);
    }

    if (mpDvdCallback != nullptr && mpDvdCallback->mDone) {
        mpDvdCallback->destroy();
        mpDvdCallback = nullptr;
        return true;
    }

    return false;
}

bool DynamicModuleControl::do_unload() {
    if (mpModule != nullptr) {
        mpHeap->free(mpModule);
        mpModule = nullptr;
    }
    return true;
}

void DynamicModuleControl::dump() {}

bool DynamicModuleControl::do_link() {
    size_t fixSize;
    void *bssStart;
    size_t blockSize;
    if (mpModule == nullptr) {
        goto fail;
    }

    fixSize = ROUND_UP(mpModule->fixSize, 0x20);
    bssStart = (void *) ((u32) mpModule + fixSize);
    blockSize = EGG::ExpHeap::getSizeForMBlock(mpModule);
    if (blockSize == 0) {
        void *bss = mpHeap->alloc(mpModule->bssSize, 0x20);
        if (bss == nullptr) {
            goto fail;
        }
        mpBss = bss;
        if (!OSLink(&mpModule->info, bss)) {
            goto fail;
        }
    } else if (fixSize + mpModule->bssSize < blockSize) {
        if (!OSLinkFixed(&mpModule->info, bssStart)) {
            goto fail;
        }
        mpHeap->resizeForMBlock(mpModule, fixSize + mpModule->bssSize);
    } else if (mpHeap->resizeForMBlock(mpModule, fixSize + mpModule->bssSize) > 0) {
        if (!OSLinkFixed(&mpModule->info, bssStart)) {
            goto fail;
        }
    } else {
        void *bss = mpHeap->alloc(mpModule->bssSize, 0x20);
        if (bss == nullptr) {
            goto fail;
        }
        mpBss = bss;
        if (!OSLinkFixed(&mpModule->info, mpBss)) {
            goto fail;
        }
        mpHeap->resizeForMBlock(mpModule, fixSize);
    }
    sAllocBytes += getModuleSize();

    // Load map file
    char mapFileBuf[0x40];
    snprintf(mapFileBuf, ARRAY_SIZE(mapFileBuf), "%s/%sNP.map", "/maps", mModuleName);
    mDebugMapFile.RegisterOnDvd(mapFileBuf, &mpModule->info);

    // Run prolog
    mPrologReturn = ((u32FctPtr) mpModule->prolog)();

    checkHeapStatus();
    return true;

fail:
    mLoadCount = 0;
    if (mpBss != nullptr) {
        mpHeap->free(mpBss);
        mpBss = nullptr;
    }
    if (mpModule != nullptr) {
        mpHeap->free(mpModule);
        mpModule = nullptr;
    }
    return false;
}

bool DynamicModuleControl::do_unlink() {
    ((voidFctPtr) mpModule->epilog)();

    mDebugMapFile.Unregister();
    if (!OSUnlink(&mpModule->info)) {
        return false;
    }

    sAllocBytes -= getModuleSize();
    if (mpBss != nullptr) {
        mpHeap->free(mpBss);
        mpBss = nullptr;
    }
    return true;
}

size_t DynamicModuleControl::getModuleSize() const {
    if (mpModule != nullptr) {
        size_t size = EGG::ExpHeap::getSizeForMBlock(mpModule);
        if (mpBss) {
            size += mpModule->bssSize;
        }
        return size;
    }
    return 0;
}

const char *DynamicModuleControl::getModuleTypeString() const {
    static const char *c_ModuleTypeStrings[] = { "????", "MEM", "ARAM", "DVD" };
    return c_ModuleTypeStrings[mType & 0b11];
}

DbMapFile::~DbMapFile() {
    if (mpMapFile != nullptr) {
        Unregister();
    }
}

void DbMapFile::RegisterOnDvd(const char *path, const OSModuleInfo *moduleInfo) {
    mpMapFile = nw4r::db::MapFile_RegistOnDvd(&mBuf, path, moduleInfo);
}

void DbMapFile::Unregister() {
    nw4r::db::MapFile_Unregist(mpMapFile);
    mpMapFile = nullptr;
}

extern "C" {

void ModuleUnresolved() {}

void ModuleConstructorsX(voidFctPtr *ctors) {
    while (*ctors != nullptr) {
        (*ctors)();
        ctors++;
    }
}

void ModuleDestructorsX(voidFctPtr *dtors) {
    while (*dtors != nullptr) {
        (*dtors)();
        dtors++;
    }
}

} // extern "C"