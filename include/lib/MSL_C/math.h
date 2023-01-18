#pragma once
///@file

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Returns the floating-point remainder of @p numer/denom (rounded towards zero).
double fmod(double x, double denom);

/// @brief Returns the absolute value of x.
inline double fabs(double x) {
    return __fabs(x);
}

/// @brief Returns the absolute value of x.
inline float fabsf(float x) {
    return __fabs(x);
}

#ifdef __cplusplus
}
#endif
