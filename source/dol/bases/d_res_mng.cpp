#include <dol/bases/d_res_mng.hpp>
#include <lib/nw4r/g3d/res_file.h>

dResMng_c *dResMng_c::m_instance;

dResMng_c::dResMng_c() {
    m_instance = this;
    mRes.init(MAX_NUM_RES, &mCallback);
}

dResMng_c::~dResMng_c() {
    m_instance = nullptr;
}

void dResMng_c::resCallback_c::init(const char *name) {}

void *dResMng_c::resCallback_c::execute(void *data, u32 folderSig) {
    if (folderSig == 'g3d ') {
        // If it's a g3d file, create a ResFile for it
        nw4r::g3d::ResFile resFile = { data };
        if (resFile.CheckRevision()) {
            resFile.Init();
            resFile.Bind(resFile);
        }
    }
    // The data doesn't need to be changed, just return it
    return data;
}

void dResMng_c::setRes(const char *path, char **names, int count, EGG::Heap *heap) {
    for (int i = 0; i < count; i++) {
        mRes.setRes(names[i], path, 0, heap);
    }
}

bool dResMng_c::setRes(const char *path, const char *name, EGG::Heap *heap) {
    return mRes.setRes(name, path, 0, heap);
}
