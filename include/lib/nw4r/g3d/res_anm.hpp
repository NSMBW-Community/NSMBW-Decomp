#pragma once

#include <types.h>

namespace nw4r {
namespace g3d {

class ResAnmVis {
public:
struct data {
    char pad[0x20];
    u16 x20;
    int x24;
};
data *p;

int getX20() const { return p->x20; }
};

class ResAnmChr {
public:
struct data {
    char pad[0x20];
    u16 x20;
    int x24;
};
data *p;

int getX20() const { return p->x20; }
};

class ResAnmClr {
public:
struct data {
    char pad[0x20];
    u16 x20;
    int x24;
};
data *p;

int getX20() const { return p->x20; }
};

class ResAnmTexPat {
public:
struct data {
    char pad[0x30];
    u16 x30;
    int _;
    int x38;
};
data *p;

int getX30() const { return p->x30; }
};

class ResAnmTexSrt {
public:
struct data {
    char pad[0x20];
    u16 x20;
    int _;
    int x28;
};
data *p;

int getX20() const { return p->x20; }
};

} // namespace g3d
} // namespace nw4r
