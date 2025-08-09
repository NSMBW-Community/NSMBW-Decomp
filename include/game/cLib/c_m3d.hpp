#pragma once
#include <types.h>
#include <revolution/MTX.h>
/// @file
/// @details [cM3d stands for C Math 3D. It is unknown why a namespace wasn't used].

/// @addtogroup clib
/// @{

/// @brief Obtains a position given a starting point, a direction and a scalar multiplier.
/// @details The function can be summed up as: @p pos = @p dir * @p mult + @p start
void cM3d_calcInDivPos1(const Vec *start, const Vec *dir, float mult, Vec *pos);

/// @brief Obtains an intermediate position between two given points using a scalar multiplier.
/// @details The function can be summed up as: @p pos = ( @p b - @p a ) * @p mult + @p a
void cM3d_calcInDivPos2(const Vec *a, const Vec *b, float mult, Vec *pos);

/// @}
