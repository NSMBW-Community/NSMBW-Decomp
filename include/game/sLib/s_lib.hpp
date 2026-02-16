#pragma once
#include <types.h>

namespace sLib {

/*
template <typename T>
T calcTimer(T* timer) {
    if (*timer != 0) {
        (*timer)--;
    }
    return *timer;
}
*/

template <typename T>
inline T targetDelta(T *value, T target) {
    return (T)(*value - target);
}

float addCalc(float *curr, float target, float speed, float maxStep, float minStep);

template <typename T>
T addCalcAngleT(T *curr, T target, T divisor, T maxStep, T minStep);
s16 addCalcAngle(s16 *curr, s16 target, s16 divisor, s16 maxStep, s16 minStep);

template <typename T>
void addCalcAngleT(T *curr, T target, T divisor, T something);
void addCalcAngle(s16 *, s16, s16, s16);

template <typename T>
int chaseT(T *, T, T);
int chase(s16 *, s16, s16);
int chase(int *, int, int);
int chase(long *, long, long);
int chase(float *, float, float);

bool chaseAngle(s16 *, s16, s16);
bool chaseAngleByRotDir(s16 *, s16, s16);

} // namespace sLib
