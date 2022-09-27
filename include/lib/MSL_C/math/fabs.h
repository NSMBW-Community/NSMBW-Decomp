#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

inline double fabs(double x) {
    return __fabs(x);
}

inline float fabsf(float x) {
    return __fabs(x);
}

#ifdef __cplusplus
}
#endif
