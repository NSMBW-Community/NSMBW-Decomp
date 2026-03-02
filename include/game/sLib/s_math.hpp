#pragma once

namespace sLib {

bool chase(short *value, short target, short step);
bool chase(int *value, int target, int step);
bool chase(long *value, long target, long step);
bool chase(float *value, float target, float step);
short addCalcAngle(short *, short, short, short, short);
void addCalc(float *, float, float, float, float);

template <typename T>
T calcTimer(T *val) {
    if (*val != 0) {
        *(val) -= 1;
    }
    return *val;
}

} // namespace sLib
