#pragma once

#include <types.h>

namespace nw4r {
namespace g3d {

enum ResAnmPolicy {
    ANM_POLICY_ONCE,
    ANM_POLICY_LOOP,
};

class ResAnmVis {
public:
struct data {
    char pad[0x20];
    u16 mDuration;
    ResAnmPolicy mAnimateType;
};
data *p;

int getDuration() const { return p->mDuration; }
};

class ResAnmChr {
public:
struct data {
    char pad[0x20];
    u16 mDuration;
    ResAnmPolicy mAnimateType;
};
data *p;

int getDuration() const { return p->mDuration; }
};

class ResAnmClr {
public:
struct data {
    char pad[0x20];
    u16 mDuration;
    ResAnmPolicy mAnimateType;
};
data *p;

int getDuration() const { return p->mDuration; }
};

class ResAnmTexPat {
public:
struct data {
    char pad[0x30];
    u16 mDuration;
    int _;
    ResAnmPolicy mAnimateType;
};
data *p;

int getDuration() const { return p->mDuration; }
};

class ResAnmTexSrt {
public:
struct data {
    char pad[0x20];
    u16 mDuration;
    int _;
    ResAnmPolicy mAnimateType;
};
data *p;

int getDuration() const { return p->mDuration; }
};

} // namespace g3d
} // namespace nw4r
