#include <types.h>
#include <game/sLib/s_StateID.hpp>

sStateID_c::NumberMemo_c sStateID_c::sm_numberMemo;
sStateID_c sStateID::null(nullptr);

sStateID_c::sStateID_c(const char *name) {
    mpName = name;
    mNumber = (name != nullptr) ? sm_numberMemo.get() : 0;
}

sStateID_c::~sStateID_c() {}

bool sStateID_c::isNull() const {
    return mNumber == 0;
}

bool sStateID_c::isEqual(const sStateIDIf_c &other) const {
    return number() == other.number();
}

int sStateID_c::operator==(const sStateIDIf_c &other) const {
    return isEqual(other);
}

int sStateID_c::operator!=(const sStateIDIf_c &other) const {
    return !isEqual(other);
}

bool sStateID_c::isSameName(const char *name) const {
    // Just return false, the full implementation is in sFStateID_c
    return false;
}

const char *sStateID_c::name() const {
    return mpName;
}

unsigned int sStateID_c::number() const {
    return mNumber;
}
