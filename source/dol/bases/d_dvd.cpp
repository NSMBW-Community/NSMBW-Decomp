#include <game/bases/d_dvd.hpp>

namespace dDvd {

void (*loader_c::m_DoneCallback)(const char *, EGG::Heap *) = nullptr;

void create(long id, EGG::Heap *heap1, EGG::Heap *heap2) {
    mDvd::create(id, heap1, heap2, nullptr);
    fn_8016B1E0();
    mDvd::setAutoStreamDecomp(true);
}

// A constructor initializer list forces field initialization in declaration order (mSize first),
// which does not match the target's store order below. Assign in the body instead.
loader_c::loader_c() {
    mpCommand = nullptr;
    mSize = -1;
    mpHeap = nullptr;
    mpBuffer = nullptr;
}

loader_c::~loader_c() {}

void *loader_c::request(const char *name, u8 allocDirection, EGG::Heap *heap) {
    if (mpBuffer) {
        return mpBuffer;
    }

    if (!mpCommand) {
        mSize = -1;
        heap = heap ? heap : mDvd::getArchiveHeap();
        mpHeap = heap;
        mpCommand = mDvd_toMainRam_c::create(name, allocDirection, heap);
    }

    if (mpCommand && mpCommand->isDone()) {
        mpBuffer = mpCommand->mpData;
        mSize = mpCommand->mSize;
        mpCommand->mpData = nullptr;
        mpCommand->destroy();
        mpCommand = nullptr;

        if (m_DoneCallback) {
            m_DoneCallback(name, mpHeap);
        }

        return mpBuffer;
    }

    return nullptr;
}

bool loader_c::remove() {
    if (!cancelCommand()) {
        return false;
    }

    mpBuffer = nullptr;
    mpHeap = nullptr;
    mSize = -1;
    return true;
}

bool loader_c::freeResouce() {
    if (!cancelCommand()) {
        return false;
    }

    freeHeap();

    mpBuffer = nullptr;
    mpHeap = nullptr;
    mSize = -1;
    return true;
}

void loader_c::freeHeap() {
    if (mpHeap && mpBuffer) {
        mpHeap->free(mpBuffer);
        mpBuffer = nullptr;
        mSize = -1;
    }
}

bool loader_c::cancelCommand() {
    if (mpCommand) {
        if (!request(nullptr, 0, nullptr)) {
            return false;
        }
        freeHeap();
    }
    return true;
}

} // namespace dDvd
