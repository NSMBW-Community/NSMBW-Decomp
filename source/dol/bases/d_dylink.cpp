#include <game/bases/d_dylink.hpp>
#include <game/mLib/m_heap.hpp>
#include <constants/sjis_constants.h>

DynamicModuleControl s_ProfileDMC("d_profile", nullptr);

const sDynNameTableEntry DynamicNameTable[] = {
    { fProfile::INVALID, nullptr }
};

const sDynNameTableEntry *pDynamicNameTable;
int nDynamicNameTable;

DynamicModuleControlBase **dDyl::pDMC;
int dDyl::nDMC;
EGG::FrmHeap *dDyl::cCc_frmHeap;
BOOL dDyl::Initialized;
mDvd_callback_c *dDyl::DVD;

bool dDyl::Init(int profileCount, const sDynNameTableEntry *pNameTable, int nNameTable, EGG::Heap *heap) {
    cCc_frmHeap = mHeap::createFrmHeap(profileCount * 16 + nNameTable * sizeof(DynamicModuleControl), heap, DYL_FRM_HEAP_NAME, 0x20, mHeap::OPT_NONE);
    EGG::Heap *prevHeap = mHeap::setCurrentHeap(cCc_frmHeap);

    nDMC = profileCount;
    pDMC = new DynamicModuleControlBase *[profileCount];
    memset(pDMC, 0, nDMC * sizeof(DynamicModuleControlBase *));

    pDynamicNameTable = pNameTable;
    nDynamicNameTable = nNameTable;
    for (int i = 0; i < nDynamicNameTable; i++) {
        const sDynNameTableEntry *curr = &pDynamicNameTable[i];
        if (curr->mModuleName == nullptr) {
            continue;
        }
        for (int j = 0; j < nDMC; j++) {
            if (pDMC[j] == nullptr) {
                continue;
            }
            if (strcmp(curr->mModuleName, pDMC[j]->getModuleName()) == 0) {
                pDMC[curr->mProf] = pDMC[j];
                break;
            }
        }
        if (pDMC[curr->mProf] == nullptr) {
            pDMC[curr->mProf] = new DynamicModuleControl(curr->mModuleName, nullptr);
        }
    }

    cCc_frmHeap->adjust();
    mHeap::setCurrentHeap(prevHeap);
    return true;
}

bool dDyl::Unlink(ProfileName profile) {
    // [This used to actually deal with unlinking before this functionality was scrapped].
    return true;
}

int dDyl::LinkASync(ProfileName profile) {
    // [This used to actually deal with linking before this functionality was scrapped].
    return Initialized ? 1 : 0;
}

void *DynamicModuleCallback::InitCallback(void *heap) {
    DynamicModuleControl::initialize(mHeap::g_dylinkHeap);
    DynamicModuleControl::sModulesDir = "/rels";

    // Load string table file
    if (DynamicModuleControl::sDvdFile->openFilename("/WIIMJ2DNP.str")) {
        size_t fileSize = DynamicModuleControl::sDvdFile->getFileSize();
        if (fileSize != 0) {
            void *stringTableBuffer = DynamicModuleControl::sDylinkHeap->alloc(ROUND_UP(fileSize, 0x20), 0x20);
            DynamicModuleControl::sDvdFile->readData(stringTableBuffer, ROUND_UP(fileSize, 0x20), 0);
            OSSetStringTable(stringTableBuffer);
        }
        DynamicModuleControl::sDvdFile->close();
    }

    bool res = s_ProfileDMC.link();
    while (!res); // Infinite loop if linking fails

    dDyl::Initialized = true;
    return (void *) true;
}

void dDyl::InitAsync() {
    Init(fProfile::PROFILE_COUNT, DynamicNameTable, ARRAY_SIZE(DynamicNameTable), mHeap::g_dylinkHeap);
    DVD = mDvd_callback_c::create(DynamicModuleCallback::InitCallback, mHeap::g_dylinkHeap);
}

bool dDyl::InitAsyncIsDone() {
    if (DVD == nullptr) {
        return true;
    }
    if (DVD->mDone) {
        DVD->destroy();
        DVD = nullptr;
        return true;
    }
    return false;
}

dDynamicModuleControl::~dDynamicModuleControl() {}

dDynamicModuleControl::dDynamicModuleControl(const char *name, EGG::ExpHeap *heap) : DynamicModuleControl(name, heap) {}

bool dDynamicModuleControl::do_link() {
    return DynamicModuleControl::do_link();
}
