#include <game/sLib/s_lib.hpp>

float sLib::addCalc(float *curr, float target, float speed, float maxStep, float minStep) {
    if (*curr != target) {
        float step = speed * (target - *curr);

        if (step >= minStep || step <= -minStep) {
            step = (step > maxStep) ? maxStep : step;

            if (step < -maxStep) {
                step = -maxStep;
            }

            *curr += step;

        } else if (step > 0.0f) {

            if (step < minStep) {
                *curr += minStep;
                if (*curr > target) {
                    *curr = target;
                }
            }

        } else {

            if (step > -minStep) {
                *curr += -minStep;
                if (*curr < target) {
                    *curr = target;
                }
            }
        }
    }

    float dist = target - *curr;
    return (dist > 0.0f) ? dist : -dist;
}

template <typename T>
T sLib::addCalcAngleT(T *curr, T target, T divisor, T maxStep, T minStep) {

    T dist = (target - *curr);
    if (*curr != target) {
        T step = dist / divisor;

        if (step > minStep || step < -minStep) {
            if (step > maxStep) {
                step = maxStep;
            }
            else if (step < -maxStep) {
                step = -maxStep;
            }

            *curr += step;

        } else if (dist >= (T)0) {
            *curr += minStep;
            if ((T)(target - *curr) <= 0) {
                *curr = target;
            }

        } else {
            *curr -= minStep;
            if ((T)(target - *curr) >= 0) {
                *curr = target;
            }
        }
    }

    return target - *curr;
}

s16 sLib::addCalcAngle(s16 *curr, s16 target, s16 divisor, s16 maxStep, s16 minStep) {
    return addCalcAngleT<s16>(curr, target, divisor, maxStep, minStep);
}

template <typename T>
void sLib::addCalcAngleT(T *curr, T target, T divisor, T something) {
  
    T step = (T)(target - *curr) / divisor;
    if (step > something) {
        *curr += something;
        return;
    }
    if (step < -something) {
        *curr -= something;
        return;
    }

    *curr += step;
}

void sLib::addCalcAngle(s16 *curr, s16 target, s16 divisor, s16 something) {
    addCalcAngleT<s16>(curr, target, divisor, something);
}

template <typename T>
int sLib::chaseT(T *curr, T target, T step) {

    if (*curr == target) {
        return true;
    }

    if (step) {
        if (*curr > target) {
            step = -step;
        }

        *curr += step;

        if (step * (*curr - target) >= 0) {
            *curr = target;
            return true;
        }
    }

    return false;
}

int sLib::chase(s16 *p1, s16 p2, s16 p3) {
    return sLib::chaseT<s16>(p1, p2, p3);
}

int sLib::chase(int *p1, int p2, int p3) {
    return sLib::chaseT<int>(p1, p2, p3);
}

int sLib::chase(long *p1, long p2, long p3) {
    return sLib::chaseT<long>(p1, p2, p3);
}

int sLib::chase(float *p1, float p2, float p3) {
    return sLib::chaseT<float>(p1, p2, p3);
}

bool sLib::chaseAngle(s16 *variable, s16 target, s16 increment) {

    if (*variable == target) {
        return true;
    }
    if (increment != 0) {
        if (targetDelta(variable, target) > 0) {
            increment = -increment;
        }

        *variable += increment;

        if (increment * targetDelta(variable, target) >= 0) {
            *variable = target;
            return true;
        }
    }

    return false;
}

bool sLib::chaseAngleByRotDir(s16 *variable, s16 target, s16 increment) {

    if (*variable == target) {
        return true;
    }

    if (increment != 0) {
        s16 step = increment;

        if (increment < 0) {
            increment = (increment == 0x8000) ? 0x7fff : (s16)-increment;
        }

        s16 r0 = increment;
        if (targetDelta(variable, target) > 0) {
            r0 = -r0;
        }

        *variable += step;

        if (step * r0 > 0) {
            if (step * targetDelta(variable, target) >= 0) {
                *variable = target;
                return true;
            }
        }
    }

    return false;
}
