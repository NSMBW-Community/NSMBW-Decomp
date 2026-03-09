#include <game/sLib/s_lib.hpp>

float sLib::addCalc(float *value, float target, float smoothing, float maxStep, float minStep) {
    if (*value != target) {
        float dist = target - *value;
        float step = smoothing * dist;

        if (step >= minStep || step <= -minStep) {
            if (step > maxStep) {
                step = maxStep;
            }
            if (step < -maxStep) { // [Possible optimization: add else to prevent unnecessary extra evaluation]
                step = -maxStep;
            }

            *value += step;

        } else if (step > 0.0f) {
            if (step < minStep) { // [Possible optimization: this check is always true]
                *value += minStep;
                if (*value > target) {
                    *value = target;
                }
            }

        } else {
            if (step > -minStep) { // [Possible optimization: this check is always true]
                *value += -minStep;
                if (*value < target) {
                    *value = target;
                }
            }
        }
    }

    // [Possible optimization: use fabsf]
    float dist = target - *value;
    return (dist > 0.0f) ? dist : -dist;
}

template <typename T>
T sLib::addCalcAngleT(T *value, T target, T smoothing, T maxStep, T minStep) {
    T dist = target - *value; // [Possible optimization: move this declaration inside the if block]
    if (*value != target) {
        T step = dist / smoothing;

        if (step > minStep || step < -minStep) {
            if (step > maxStep) {
                step = maxStep;
            }
            else if (step < -maxStep) {
                step = -maxStep;
            }

            *value += step;

        } else if (dist >= 0) {
            *value += minStep;
            dist = target - *value;
            if (dist <= 0) {
                *value = target;
            }

        } else {
            *value -= minStep;
            dist = target - *value;
            if (dist >= 0) {
                *value = target;
            }
        }
    }

    return target - *value;
}

s16 sLib::addCalcAngle(s16 *value, s16 target, s16 smoothing, s16 maxStep, s16 minStep) {
    return addCalcAngleT<s16>(value, target, smoothing, maxStep, minStep);
}

template <typename T>
void sLib::addCalcAngleT(T *value, T target, T smoothing, T maxStep) {
    T dist = target - *value;
    T step = dist / smoothing;

    if (step > maxStep) {
        *value += maxStep;
    } else if (step < -maxStep) {
        *value -= maxStep;
    } else {
        *value += step;
    }
}

void sLib::addCalcAngle(s16 *value, s16 target, s16 smoothing, s16 maxStep) {
    addCalcAngleT<s16>(value, target, smoothing, maxStep);
}

template <typename T>
BOOL sLib::chaseT(T *value, T target, T step) {
    if (*value == target) {
        return TRUE;
    }

    if (step) {
        if (*value > target) {
            step = -step;
        }

        *value += step;
        if (step * (*value - target) >= 0) {
            *value = target;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL sLib::chase(s16 *value, s16 target, s16 step) {
    return sLib::chaseT<s16>(value, target, step);
}

BOOL sLib::chase(int *value, int target, int step) {
    return sLib::chaseT<int>(value, target, step);
}

BOOL sLib::chase(long *value, long target, long step) {
    return sLib::chaseT<long>(value, target, step);
}

BOOL sLib::chase(float *value, float target, float step) {
    return sLib::chaseT<float>(value, target, step);
}

BOOL sLib::chaseAngle(s16 *value, s16 target, s16 step) {
    if (*value == target) {
        return TRUE;
    }

    if (step != 0) {
        s16 dist = *value - target;
        if (dist > 0) {
            step = -step;
        }

        *value += step;
        dist = *value - target;

        if (step * dist >= 0) {
            *value = target;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL sLib::chaseAngleByRotDir(s16 *value, s16 target, s16 step) {
    if (*value == target) {
        return TRUE;
    }

    if (step != 0) {
        s16 increment = step;

        // Get absolute step value, accounting for overflow
        if (step < 0) {
            step = (step == 0x8000) ? 0x7fff : (s16)-step;
        }

        // If the distance to the target is negative, then negate step to move towards it
        s16 dist = *value - target;
        if (dist > 0) {
            step = -step;
        }

        *value += increment;

        // Check if the target's direction matches the intended direction
        // If so, perform the overshoot check
        if (increment * step > 0) {
            s16 dist = *value - target;
            if (increment * dist >= 0) {
                *value = target;
                return TRUE;
            }
        }
    }

    return FALSE;
}
